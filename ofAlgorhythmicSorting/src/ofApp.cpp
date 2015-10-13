#include "ofApp.h"

#include "ofxSimpleGuiToo.h"
#include "ofxSimpleGuiComboBox.h"

#include "SortingMan.h"
#include "BarsView.h"
#include "SquaresView.h"
#include "ConsoleView.h"
#include "ImageWriterSquaresView.h"

#include "handyfuncs.h"
#include "AlgorhythmicAudioIO.h"

#define kSquaresViewIndex 0
#define kBarsViewIndex    1
#define kConsoleViewIndex 2

enum ArrayFillTypes{
    eRandomArray = 0,
    eSortedArray = 1,
    eReversedArray,
    eDiscreteRandomArray,
    eDiscrete3Array,
    eDiscrete5Array,
    eDiscreteSortedArray,
    eDiscreteReversedArray,
    eNumArrayFillTypes
};

static const char* arrayfillstrings[]=
{
    "Random",
    "Sorted",
    "Reversed",
    "Discrete Random",
    "Discrete 3",
    "Discrete 5",
    "Discrete Sorted",
    "Discrete Reversed"
};

float speedSliderVal;
float speedSliderVal_old;
int myInt1;
bool startButton;
bool genButton;
bool bSaveImage = false;
bool bToggleFullscreen = false;
string textInput;

ofxSimpleGuiButton * startButtonControl;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    currentFontSize = 10;
    arrayFont = new ofTrueTypeFont();
    arrayFont->loadFont("Inconsolata.otf", currentFontSize);
    
    gui.setup();
    ofxSimpleGuiConfig & config = *gui.config;
    
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
    arraysize = prev_arraysize = 10;
    freqRangeSliderVal = 0.20f;
    freqBaseSliderVal  = 100.0f;
    volumeSliderVal    = 0.8f;
    bPercussion        = true;
    bTones             = true;
    bLoop              = false;
    gui.addSlider("Speed", speedSliderVal, 1.0f, 500.0f);//, false);
    gui.addSlider("Volume", volumeSliderVal, 0.0f, 1.0f);//, false);
    gui.addSlider("Frequency Range", freqRangeSliderVal, 0.001f, 1.0f);//, false);
    gui.addSlider("Frequency Base", freqBaseSliderVal, 40.0f, 20000.0f);//, false);
    gui.addToggle("Percusion", bPercussion);
    gui.addToggle("Tones", bTones);
	gui.addToggle("Loop", bLoop);
    
    string algostrings[eNumSortAlgorithms];
    for(int i=0;i<eNumSortAlgorithms;i++){
        algostrings[i]=gAlgoStrings[i];
    }
    
    gui.addComboBox("Sorting Algorithm", algonum, eNumSortAlgorithms,algostrings);
    
    gui.addSlider("Array Size", arraysize, 2, 200);
    
    string arraytypestrings[eNumArrayFillTypes];
    for(int i=0;i<eNumArrayFillTypes;i++){
        arraytypestrings[i] = arrayfillstrings[i];
    }
    
    gui.addComboBox("Initial Array Type", arraytypenum, eNumArrayFillTypes, arraytypestrings);
    
    gui.addButton("Generate New Array", genButton);
	startButtonControl = &gui.addButton("Start", startButton);
    
	gui.addButton("Toggle Fullscreen", bToggleFullscreen);
	gui.addButton("Save History as Image", bSaveImage);
    
    
    gui.loadFromXML();
    
    gui.setPage(2);
    gui.show();
    
    drawGUI = true;
    
    //flags  init
    mouseMode      = eMouseModeNone;
    keyMode        = eKeyModeNormal;
    activeEffects  = eAudioEffectsAll;
    audioState     = eAudioStateNone;
    
    //setup views
    views.push_back(new SquaresView(0,0,ofGetWidth(),ofGetHeight()));
    views.push_back(new BarsView(0,0,ofGetWidth(),ofGetHeight()));
    views.push_back(new ConsoleView(0,0,ofGetWidth(),ofGetHeight()));
    viewMode = eDrawBarsAndSquares;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(bSaveImage) {
        writePng();
        bSaveImage = false;
    }
    if (bToggleFullscreen) {
        ofToggleFullscreen();
        ofSetVerticalSync(true);
        bToggleFullscreen = false;
    }
    
    if(startButton) {
        startButton = false;
        //start.
        if(SortingMan::Instance()->IsSorting()) {
            SortingMan::Instance()->StopSorting();
            startButtonControl->setName("start");
        } else {
            if (prev_arraysize != arraysize) {
                SortingMan::Instance()->GenerateNewArray(arraysize);
                prev_arraysize = arraysize;
            }
            SortingMan::Instance()->SortStuff();
            startButtonControl->setName("stop");
        }
    }else if(!SortingMan::Instance()->IsSorting()){
        if (startButtonControl->name.compare("stop") == 0)
            startButtonControl->setName("start");
    }
    
    if(genButton){
        genButton = false;
        SortingMan::Instance()->GenerateNewArray(arraysize);
    }
    
    SortingMan::Instance()->SetAlgorithm((SortAlgorithms)algonum);
    bool presort = false;
    bool reverse = false;
    int discreteElements = 0;
    
    if (arraytypenum == eSortedArray || arraytypenum == eDiscreteSortedArray)
        presort = true;
    if (arraytypenum == eReversedArray || arraytypenum == eDiscreteReversedArray)
        reverse = true;
    if (arraytypenum == eDiscreteRandomArray || arraytypenum == eDiscreteSortedArray
        || arraytypenum == eDiscreteReversedArray)
        discreteElements = randint(20) + 1;
    if (arraytypenum == eDiscrete3Array)
        discreteElements = 3;
    if (arraytypenum == eDiscrete5Array)
        discreteElements = 5;
    
    SortingMan::Instance()->SetArrayFillStyle(presort, reverse, discreteElements);
    if(bLoop != SortingMan::Instance()->IsLooping())
        SortingMan::Instance()->ToggleLooping();
    
    //stupid hack to make sliders work with functions
    if(speedSliderVal != speedSliderVal_old) {
        speedSliderVal_old = speedSliderVal;
        AlgorhythmicAudioIO::Instance()->ChangeSpeed(speedSliderVal/1000);
    }
    
    //just constantly blast the sonification params
    AlgorhythmicAudioIO::Instance()->ChangeFreqMin(freqBaseSliderVal);
    AlgorhythmicAudioIO::Instance()->ChangeFreqMax(freqRangeSliderVal *
                                                   (20000.0f - freqBaseSliderVal));
    AlgorhythmicAudioIO::Instance()->SetNotes(bTones);
    AlgorhythmicAudioIO::Instance()->SetSeperator(bPercussion);
    AlgorhythmicAudioIO::Instance()->ChangeVolume(volumeSliderVal);
}

//--------------------------------------------------------------
void ofApp::draw(){
    int i = 0;
    switch (viewMode) {
        case eDrawAll:
            views[kSquaresViewIndex]->SetRect(0, 0, ofGetWidth() / 2, ofGetHeight() / 2);
            views[kBarsViewIndex]->SetRect(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
            views[kConsoleViewIndex]->SetRect(0, ofGetHeight() / 2, ofGetWidth(), ofGetHeight() /2);
            fetchHistory();
            for(i=0;i<views.size();i++){
                views[i]->Draw(history, markers);
            }
            break;
        case eDrawBarsAndSquares:
            views[kSquaresViewIndex]->SetRect(0, 0, ofGetWidth() / 2, ofGetHeight());
            views[kBarsViewIndex]->SetRect(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight());
            fetchHistory();
            views[kSquaresViewIndex]->Draw(history, markers);
            views[kBarsViewIndex]->Draw(history, markers);
            break;
        case eDrawSquares:
            views[kSquaresViewIndex]->SetRect(0, 0, ofGetWidth(), ofGetHeight());
            views[kConsoleViewIndex]->SetRect(0, 0, ofGetWidth(), ofGetHeight());
            fetchHistory();
            views[kSquaresViewIndex]->Draw(history, markers);
            break;
        case eDrawBars:
            views[kBarsViewIndex]->SetRect(0, 0, ofGetWidth(), ofGetHeight());
            views[kConsoleViewIndex]->SetRect(0, 0, ofGetWidth(), ofGetHeight());
            fetchHistory();
            views[kBarsViewIndex]->Draw(history, markers);
            break;
        case eDrawConsole:
            views[kConsoleViewIndex]->SetRect(0, 0, ofGetWidth(), ofGetHeight());
            fetchHistory();
            views[kConsoleViewIndex]->Draw(history, markers);
            break;
            
        default:
            break;
    }
    
    if(drawGUI)gui.draw();
}

static char lastDrawString[kArraysStringMaxSize]="";
void ofApp::fetchHistory(){
    int rowsNeeded = 1;
    //find largest history req for our views
    for(int i=0;i<views.size();i++){
        rowsNeeded = mymax(views[i]->CalcNumRowsNeeded(), rowsNeeded);
    }
    rowsNeeded = mymax(kMinHistory, rowsNeeded);
    static int lastlen = -1;
    int len = SortingMan::Instance()->GetArraySize();
    
    double * array = NULL;
    int * marker = NULL;
    
    if(lastlen!=len){
        for(int i=0;i<history.size();i++){
            free(history[i]);
            free(markers[i]);
        }
        history.clear();
        markers.clear();
    }
    
    while(SortingMan::Instance()->PopHistory(&array, &marker)){
        history.push_back(array);
        markers.push_back(marker);
    }
    
    while (history.size() > rowsNeeded) {
        free(history[0]);
        history.erase(history.begin() + 0);
        free(markers[0]);
        markers.erase(markers.begin() + 0);
    }
    lastlen = len;
}

void ofApp::writePng(){
    //draw everything in the history
    ImageWriterSquaresView *view = new ImageWriterSquaresView(0,0,ofGetWidth(),ofGetHeight());
    ofImage image;
    int squaresDrawn;
    int width = ofGetWidth() /2 ;
    int height = ofGetHeight() /2;
    int squareHeight;
    
    view->SetRect(0, 0, width, height);
    squareHeight = view->GetSquareHeight();
    height = squareHeight * history.size() + kDrawMethodBorder * 2;
    
    image.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    view->SetImage(&image);
    view->SetRect(0, 0, width, height);
    view->Draw(history, markers);
    delete view;
    
    char filename[200];
    static int counter = 1;
    sprintf(filename, "%04dAlgorhymicSorting%d-%d-%d-%02d:%02d:%02d.png", counter++, ofGetDay(), ofGetMonth(), ofGetYear(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
    image.saveImage(filename);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '-'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+'){
		volume += 0.05;
		volume = MIN(volume, 1);
	} else if(key == 'c') {
        drawGUI=!drawGUI;
    }else if(key == '5')
    {
        viewMode = eDrawBarsAndSquares;
    }else if(key == '0')
    {
        bSaveImage = true;
    }else if(key == '4')
    {
        viewMode = eDrawAll;
    }else if(key == '3')
    {
        viewMode = eDrawSquares;
    }else if(key == '2')
    {
        viewMode = eDrawBars;
    }else if(key == '1')
    {
        viewMode = eDrawConsole;
    }else if(key == 'a')
    {
        //set mouse mode
        keyMode = eKeyModeMouse;
    }else if(key == 'z')
    {
        //set mouse mode
        keyMode = eKeyModeZoom;
    }else if(key == 'x')
    {
        keyMode = eKeyModeEffects;
    }else if(key == ' ')
    {
        startButton = 1;
    }
}

void ofApp::keyPressedEffectsMode(int key){
    if(key == 'a' || key == 'z')
    {
        ;
    }else if(key == 's')
    {
        //toggle pause input
        audioState ^= eAudioStatePauseInput;
    } else if(key == 'd')
    {
        activeEffects^= eAudioEffectsDownsample;
    } else if(key == 'f')
    {
        activeEffects ^= eAudioEffectsNoiseRing;//send noise feedback.
    }
    keyMode = eKeyModeNormal;
}

void ofApp::keyPressedZoomMode(int key){
    if(key == 'a' || key == 'z')
    {
        ringZoom = 1.0;
    }else if(key == 's')
    {
        ringZoom = ringZoom/2 < 1.0?1.0:(ringZoom/2);
    } else if(key == 'd')
    {
        ringZoom = ringZoom*2;
    } else if(key == 'f')
    {
        ringZoom = 10;
    }
    keyMode = eKeyModeNormal;
}

void ofApp::keyPressedMouseMode(int key){
    if(key == 'a')
    {
        //reset
        mouseMode = eMouseModeNone;
    }else if(key == 's')
    {
        //toggle
        mouseMode ^= eMouseModeNoise1;
    } else if(key == 'w')
    {
        mouseMode ^= eMouseModeNoise2;
    } else if(key == 'f')
    {
        mouseMode ^= eMouseModeFeedback;
    } else if(key == 'v')
    {
        mouseMode ^= eMouseModePan;
    } else if(key == 'c')
    {
        mouseMode ^= eMouseModePanNoise;
    }else if(key == 'd')
    {
        mouseMode ^= eMouseModeDownsample;
    } else if(key == 'g')
    {
        mouseMode = eMouseModeAll;
    }
    
    keyMode = eKeyModeNormal;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    int width = ofGetWidth();
	float height = (float)ofGetHeight();
    
    float cursorRatioX = (float)x / (float)width;
    float cursorRatioY = ((height-y) / height);
    
    
    ycurs = cursorRatioY;
    targetFrequency = 2000.0f * ((int)(cursorRatioY*40))/40.0f;
    
    if(mouseMode&eMouseModeFeedback)
        feedback = cursorRatioY;
    if(mouseMode&eMouseModePan)
        pan = cursorRatioX;
    if(mouseMode&eMouseModeDownsample)
    {
        downsampleTimes = cursorRatioY * 8;
    }
    
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
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

void ofApp::exit(){
    delete arrayFont;
}
