#pragma once
#define kArraysStringMaxSize 32767
#define kDrawMethodBorder 10
#define kMinHistory 1000
#define kDrawMethodBorder 10

#include "ofMain.h"

enum MouseMode {
    eMouseModeNone       = 0x0000,
    eMouseModeNoise1     = 0x0001,
    eMouseModeNoise2     = 0x0002,
    eMouseModePan        = 0x0004,
    eMouseModePanNoise   = 0x0008,
    eMouseModeFeedback   = 0x0010,
    eMouseModeDownsample   = 0x0020,
    eMouseModeAll        = 0xFFFF
};

enum KeyMode {
    eKeyModeNormal       = 0x0000,
    eKeyModeZoom         = 0x0001,
    eKeyModeMouse        = 0x0002,
    eKeyModeEffects       = 0x0004,
    eKeyModeAll        = 0xFFFF
};

enum AudioEffects {
    eAudioEffectsNone          = 0x0000,
    eAudioEffectsReverb        = 0x0001,
    eAudioEffectsDelay         = 0x0002,
    eAudioEffectsDelayFeedback = 0x0004,
    eAudioEffectsLoop          = 0x0008,
    eAudioEffectsLoopFeedback  = 0x0010,
    eAudioEffectsNoiseRing     = 0x0020,
    eAudioEffectsNoise1        = 0x0040,
    eAudioEffectsNoise2        = 0x0080,
    eAudioEffectsPanNoise      = 0x0100,
    eAudioEffectsDownsample    = 0x0200,
    eAudioEffectsUnusedFlag    = 0x0400,
    eAudioEffectsAll           = 0xFFFF
};

enum AudioState {
    eAudioStateNone       = 0x0000,
    eAudioStatePauseInput = 0x0001,
    eAudioStateAll        = 0xFFFF
};

enum ViewMode {
    eDrawConsole = 0,
    eDrawSquares = 1,
    eDrawBars,
    eDrawBarsAndSquares,
    eDrawAll
};
class ArrayView;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void keyPressedMouseMode(int key);
    void keyPressedZoomMode(int key);
    void keyPressedEffectsMode(int key);
    
    void writePng();
    void exit();
protected:
    void fetchHistory();
    
    std::vector<double*> history;
    std::vector<int*> markers; //for change, focus, etc.
    std::vector<ArrayView*> views;
    vector<double*> mHistory;
    float 	pan;
    int		sampleRate;
    float 	volume;
    float    ycurs;
    int    downsampleTimes;
    int   algonum;
    int   arraytypenum;
    int   arraysize, prev_arraysize;
    ViewMode viewMode;
    
    //gui vals
    bool bLoop;
    bool bPercussion;
    bool bTones;
    float freqRangeSliderVal;
    float freqBaseSliderVal;
    float volumeSliderVal;
    
    float 	* lAudio;
    float   * rAudio;
    float * lIn;
    float * rIn;
    float * lRing;
    float * rRing;
    int 	bufferCounter;
    int 	drawCounter;
    int   writeRingCursor;
    int   readRingCursor;
    
    //------------------- for the simple sine wave synthesis
    float 	targetFrequency;
    float 	phase;
    float 	phaseAdder;
    float 	phaseAdderTarget;
    float    playthrough;
    
    float    startMarker;
    float    endMarker;
    bool     startMarkerSet;
    bool     useLoopFade;
    
    //flags
    unsigned int   mouseMode;
    unsigned int   keyMode;
    unsigned int   activeEffects;
    unsigned int   audioState;
    
    float ringZoom;
    
    std::vector<int> delayPoints;
    std::vector<int> loopStartPoints;
    std::vector<int> loopEndPoints;
    std::vector<int> loopPoints;
    
    float feedback;
    float input1ToLLevel;
    float input1ToRLevel;
    float input2ToLLevel;
    float input2ToRLevel;
    
    float input1ToLRing;
    float input1ToRRing;
    float input2ToLRing;
    float input2ToRRing;
    
    ofMutex delayMutex;
    ofMutex loopMutex;
    
    ofTrueTypeFont * arrayFont;
    int currentFontSize;
    bool drawGUI;
		
};
