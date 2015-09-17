#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "string.h"
#include "ofxGui.h"
#include "pointCloudStitcher.h"

#define HOST "localhost"
#define PORT 12345

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
		
	void keyPressed(int key);
    
    ofColor avgColor(ofRectangle area, float offsetRatio);
    string colorNamer(ofColor toBeNamed);
    void setupGui();
    
    pointCloudStitcher kinect;
    
    // GUI Variables
    ofxPanel gui;
    ofParameter<bool> bShowInfo;
    
    ofxCvGrayscaleImage patchedImageCv, grayImage;
	
	ofxCvContourFinder contourFinder;
    
    //CompVisionShizNit
    ofxCvGrayscaleImage     grayDiffOfImage;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     bgImage;

    bool bLearnBackground;
    bool bBackgroundLearned;
    bool bForgetBackground;	
    
    ofxOscSender sender;
	
    ofColor testColor;
};
