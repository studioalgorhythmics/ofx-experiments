#include "ofApp.h"

typedef struct sieve_state {
    unsigned long long G;
    unsigned long long q;
    unsigned long long p;
    unsigned long long  rest;
    unsigned long long  lastPrime;
    
    unsigned long long  lastQ;
    float leftoverTime;
    float leftover_samples[kLeftoverSampleSize];
    int num_leftover_samples;
} sieve_state;

sieve_state curr_state;

static float ** output_buffers;
static volatile int output_buffer_cursor;
static volatile int buffers_ready;
static int pause_samps;

void sieve_step()
{
    int i;
	curr_state.lastQ = curr_state.q;
    curr_state.q    = curr_state.G / curr_state.p;
    curr_state.rest = curr_state.G % curr_state.p;
    
    curr_state.leftoverTime += kFixedTimestep;
	
	// wait till odd or even state changes and then create a sin tone.
	
#ifdef USESINTONE
	int sampsOut = 0;
	if (((curr_state.q & 1) != (curr_state.lastQ  & 1)) || curr_state.leftoverTime / (1.0f / kSampleRate)  + curr_state.num_leftover_samples > kLeftoverSampleSize / 2) {
		// now sonify based on q
		bool isOdd = curr_state.q & 1;
		sampsOut = curr_state.leftoverTime / (1.0/kSampleRate);
		curr_state.leftoverTime -= sampsOut * (1.0f / kSampleRate);
		float res;
		
		for (i = 0; i < sampsOut; i++) {
			res = curr_state.q & 1 ? 1.0f : -1.0f; // only look at the least significant bit
			curr_state.leftover_samples[i + curr_state.num_leftover_samples] = sin(((float)i/sampsOut) * 3.141592653589793238 + (isOdd ? 3.141592653589793238 : 0));
		}
		
	}
	
#else
    // now sonify based on q
    int sampsOut = curr_state.leftoverTime / (1.0/kSampleRate);
    curr_state.leftoverTime -= sampsOut * (1.0f / kSampleRate);
    float res;
    
    for (i = 0; i < sampsOut; i++) {
        res = curr_state.q & 1 ? 1.0f : -1.0f; // only look at the least significant bit
        curr_state.leftover_samples[i + curr_state.num_leftover_samples] = res;
    }
#endif
    
    curr_state.num_leftover_samples += sampsOut;
    
    //update
    if (curr_state.rest == 0 || curr_state.p > curr_state.q) { // or
        if (curr_state.rest != 0 && curr_state.p > curr_state.q) { //not
            curr_state.lastPrime = curr_state.G;
			pause_samps = kPrimeFoundPauseSecs * kSampleRate;
        }
        curr_state.p = 3;
        curr_state.G += 2;
        
    } else {
        curr_state.p += 2;
    }
}

void fill_buffer()
{
    int bufindex = (output_buffer_cursor + buffers_ready + 1) % kNumSieveOutputBuffers;
    int bufsize = AlgorhythmicAudioIO::Instance()->GetFramesPerBuffer();
    int sampindex = 0;
    int copied = 0;
    
    if (pause_samps) {
        
        copied = mymin(bufsize - sampindex, pause_samps);
        memset((output_buffers[bufindex] + sampindex), 0, copied * sizeof(float));
        sampindex += copied;
        pause_samps -= copied;
    }
    
    while (sampindex < bufsize) {
        if (curr_state.num_leftover_samples) {
            copied = mymax(0, mymin(bufsize - sampindex, curr_state.num_leftover_samples));
            memcpy((output_buffers[bufindex] + sampindex), curr_state.leftover_samples, copied * sizeof(float));
            sampindex += copied;
            curr_state.num_leftover_samples -= copied;
            
            if (curr_state.num_leftover_samples) {
                memmove(curr_state.leftover_samples, curr_state.leftover_samples + copied, sizeof(float) * curr_state.num_leftover_samples);
            }
        }
        
        if (sampindex < bufsize) {
            // do the computation
            sieve_step();
        }
    }
    
    buffers_ready++;
}

//returns 1 when filled to capacity
int  fill_buffers()
{
    if (buffers_ready < kNumSieveOutputBuffers * 0.8) {
        fill_buffer();
        return 0;
    }
    return 1;
}

float* pop_buffer()
{
    float* res = output_buffers[output_buffer_cursor];
    
    while(!fill_buffers())
        ;
    output_buffer_cursor = (output_buffer_cursor + 1) % kNumSieveOutputBuffers;
    
    buffers_ready--;
    return res;
}

void init_output_buffers()
{
    int i;
    int fpb = AlgorhythmicAudioIO::Instance()->GetFramesPerBuffer();
    
    if (!output_buffers) {
        output_buffers = new float*[kNumSieveOutputBuffers];
        for (i = 0; i < kNumSieveOutputBuffers; i++)
            output_buffers[i] = new float[fpb];
    }
}


void init_sieve()
{
    init_output_buffers();
    curr_state.G = kInitialSieveG;
    curr_state.q = 0;
    curr_state.p = 3;
    curr_state.lastPrime = 3;
    curr_state.num_leftover_samples = 0;
    curr_state.leftoverTime = 0;
    pause_samps = 0;
}



float speedSliderVal_old;
int myInt1;
bool startButton;
bool bToggleFullscreen = false;
string textInput;
ofxSimpleGuiButton *startButtonControl;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetFrameRate(60);
    
    int currFontSize = 10;
    arrayFont = new ofTrueTypeFont;
    arrayFont->loadFont("Inconsolata.otf", currFontSize, true, true);
    
    gui.setup();
    ofxSimpleGuiConfig &config = *gui.config;
    
    config.sliderHeight		= 10;
	config.sliderTextHeight	= 15;
	config.titleHeight			= config.sliderHeight + config.sliderTextHeight;
	config.toggleHeight		= config.titleHeight;
	config.buttonHeight		= config.titleHeight;
	config.slider2DTextHeight	= config.titleHeight * 1.5;
	config.comboBoxHeight		= 47;	//15
	config.comboBoxTextHeight	= 16;
	
	
	config.padding.set			(config.titleHeight/2, 10);
	config.offset.set			(config.titleHeight/2, config.titleHeight/2);
	config.slider2DSize.set	(config.titleHeight * 4, config.titleHeight * 4);
	
	config.gridSize.x			= 200 + config.padding.x;
	config.gridSize.y			= config.toggleHeight + config.padding.y;
    
    gui.addPage(" ");
    
    speedSliderVal = speedSliderVal_old = 25;
    volumeSliderVal    = 0.8f;
    gui.addSlider("Speed", speedSliderVal, 1.0f, 500.0f);
    gui.addSlider("Volume", volumeSliderVal, 0.0f, 1.0f);
    
	gui.addButton("Toggle Fullscreen", bToggleFullscreen);
    
	gui.loadFromXML();
    gui.setPage(2);
    gui.show();
    
    drawGUI=true;
    
    AlgorhythmicAudioIO::Instance();
    init_sieve();
    while(!fill_buffers());
}

//--------------------------------------------------------------
void ofApp::update(){
    if (bToggleFullscreen) {
        ofToggleFullscreen();
        ofSetVerticalSync(true);
        bToggleFullscreen = false;
    }
    
    if(startButton) {
        AlgorhythmicAudioIO::Instance()->Start();
        startButton = false;
    }
    
    AlgorhythmicAudioIO::Instance()->ChangeVolume(volumeSliderVal);
}

//--------------------------------------------------------------
void ofApp::draw(){
    char numoutbuf[100];
    
    sprintf(numoutbuf, "G %lld p %lld q %lld lastprime %lld", curr_state.G, curr_state.p, curr_state.q, curr_state.lastPrime);
    arrayFont->drawString(numoutbuf,
                          500,
                          500);
    
    if(drawGUI)gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c') {
        drawGUI=!drawGUI;
    }else if(key == ' ')
    {
        startButton = 1;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

