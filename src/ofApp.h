#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "pointCloudStitcher.h"
#include "colorNamer.h"
#include "videoProjection.h"
#include "WindowManager.h"
#include "blobTracker.h"

//#define HOST "localhost"
#define HOST "169.254.97.135"
#define PORT 12345

class ofApp : public ofBaseApp {
public:
    // ofStandardShiznit
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
    
    // window manager for second window
    WindowManager winManager;
    
    // kinectMerger
    pointCloudStitcher kinect;
    
    // GUI Variables og medd’
    void setupGui();

    ofxPanel gui;
    ofParameterGroup mainAppPm;
    ofParameter<bool> bShowInfo, bDrawProjectionMapping, bShowLabelColors;
    ofParameter<string> host;
    
    // blob tracker
    blobTracker blobTrackr; 
    
    // colorFinding
    ofColor avgColor(ofRectangle area);
    string lastFoundColorGroup;
    colorNamer clrNamer;
    
    // video projection
    videoProjection vidProjection; 
    
    // oscSender
    ofxOscSender sender;
    void oscSender();
};
