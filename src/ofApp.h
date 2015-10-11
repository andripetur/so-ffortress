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
    ofParameter<int> minArea;
    ofParameter<string> host;
    
    //CompVisionShizNit
    void drawContFinder();

    ofxCv::ContourFinder    contFinder;
    ofxCvGrayscaleImage     patchedImageCv;
    ofxCvGrayscaleImage     grayDiffOfImage;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     bgImage;
    
    bool bLearnBackground;
    bool bBackgroundLearned;
    bool bForgetBackground;

    // search zone
    ofRectangle searchZone;
    ofParameterGroup searchZonePmGroup;
    ofParameter<bool> bSearchZoneOn;
    ofParameter<float> sZwidth, sZheight;
    ofParameter<ofVec2f> szCenter;
    
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
