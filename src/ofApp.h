#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "string.h"
#include "ofxGui.h"
#include "pointCloudStitcher.h"
#include "colorNamer.h"

//#define HOST "localhost"
#define HOST "169.254.97.135"
#define PORT 12345

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
		
	void keyPressed(int key);
    
    ofColor avgColor(ofRectangle area);
    string lastFoundColorGroup;

    void setupGui();
    void oscSender();
    void drawContFinder();
    
    pointCloudStitcher kinect;
    
    // GUI Variables
    ofxPanel gui;
    ofParameter<bool> bShowInfo;
    ofParameter<int> minArea;
    ofParameter<string> host; 
    bool bShowLabels;
    
    //CompVisionShizNit
    ofxCv::ContourFinder    contFinder;
    ofxCvGrayscaleImage     patchedImageCv;
    ofxCvGrayscaleImage     grayDiffOfImage;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     bgImage;
    
    ofRectangle searchZone;
    ofParameterGroup searchZonePmGroup;
    ofParameter<bool> bSearchZoneOn;
    ofParameter<float> sZwidth, sZheight;
    ofParameter<ofPoint> szCenter;
    
    bool bLearnBackground;
    bool bBackgroundLearned;
    bool bForgetBackground;	
    
    ofxOscSender sender;
    colorNamer clrNamer; 
};
