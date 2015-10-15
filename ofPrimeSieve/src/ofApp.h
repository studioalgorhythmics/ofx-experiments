#pragma once

#include "ofMain.h"
#include "ofxSimpleGuiToo.h"
#include "handyfuncs.h"
#include "AlgorhythmicAudioIO.h"

#define kLeftoverSampleSize 8192
#define kSampleRate (44100.0)
#define kNumSieveOutputBuffers 18
#define kFixedTimestep (180/1000000.0f) //  (180/1000000.0f) // (10/1000000.0f)
#define kInitialSieveG 34359738369ULL //34359738369LLU // 33333333444433LLU
#define kPrimeFoundPauseSecs 1.0f // 2.0f
#define kArraysStringMaxSize 32767
#define kDrawMethodBorder 10

#define USESINTONE 1

float * pop_buffer();

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
		
   
    
protected:
    float speedSliderVal, volumeSliderVal;
    ofTrueTypeFont   *arrayFont;
    int               currFontSize;
    bool drawGUI;
};
