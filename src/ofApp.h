#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "string.h"
#include "ofxGui.h"
#include "pointCloudStitcher.h"
#include "colorNamer.h"

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
    void setupGui();
    void oscSender();
    void drawContFinder();
    
    pointCloudStitcher kinect;
    
    // GUI Variables
    ofxPanel gui;
    ofParameter<bool> bShowInfo;
    ofParameter<int> minArea;
    bool bShowLabels;
    
    //CompVisionShizNit
    ofxCv::ContourFinder    contFinder;
    ofxCvGrayscaleImage     patchedImageCv;
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayDiffOfImage;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     bgImage;

    bool bLearnBackground;
    bool bBackgroundLearned;
    bool bForgetBackground;	
    
    ofxOscSender sender;
    colorNamer clrNamer; 
	
    ofColor testColor;
    bool bColorChanged; 
    string colorName; 
};
