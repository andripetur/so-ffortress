#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "string.h"
#include "ofxGui.h"

#define HOST "localhost"
#define PORT 12345

// uncomment this to read from two kinects simultaneously
#define USE_TWO_KINECTS

#ifdef USE_TWO_KINECTS
#define amt 2
#else
#define amt 1
#endif

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
	void keyPressed(int key);
    
    ofColor avgColor(ofRectangle area, float offsetRatio);
    string colorNamer(ofColor toBeNamed);
    float** mergePointclouds(ofxKinect* kinectOnePtr, ofxKinect* kinectTwoPtr);
    void mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float ** adj);
    void setupGui();
    
    // GUI Variables
    ofxPanel gui;
    ofParameter<float> mapFromLowZ, mapFromHighZ, mapToLowZ, mapToHighZ, transformConstant, pointSize;
    ofParameter<int> kinectDistance, kinectDistanceY, stepSize, pointCloudFarClipping;
    ofParameter<bool> bEnableMouseInput, bShowInfo;
    ofParameterGroup patchCloudParameters;
    
    void renderPatchCloud();
    ofFbo patchedImage;
    ofPixels patchedImagePixels; 
    ofxCvGrayscaleImage patchedImageCv; 

    ofxKinect kinect[amt];
	
	ofxCvContourFinder contourFinder;
	
	bool bDrawPointCloud;
    
    //CompVisionShizNit
    ofxCvGrayscaleImage     grayImage[amt]; // grayscale depth image
    ofxCvGrayscaleImage     grayDiffOfImage[amt];
    ofxCvGrayscaleImage 	grayDiff[amt];
    ofxCvGrayscaleImage     bgImage[amt];

    bool bLearnBackground[amt];
    bool bBackgroundLearned[amt];
    bool bForgetBackground[amt];
    bool bWorldToCamera = false; 
	
	int angle;
    
    ofxOscSender sender;
	
    ofColor testColor;
	// used for viewing the point cloud
	ofEasyCam easyCam;
    ofMesh mesh; 
};
