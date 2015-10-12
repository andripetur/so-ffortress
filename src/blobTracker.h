//
//  blobTracker.h
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 11/10/15.
//
//
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "searchZone.h"

#ifndef __ffortress__blobTracker__
#define __ffortress__blobTracker__

class blobTracker{
public:
    blobTracker(){
        ofAddListener(ofEvents().keyPressed, this, &blobTracker::keyListener);
    }
    void setup(ofVec2f kinectWidthHeigt); // kwh = kinect width and height
    void setupParameters(); 
    void update(ofxCvGrayscaleImage depthImage);
    void drawContourFinder();
    void draw();
    void keyListener(ofKeyEventArgs & a);
    void calculateMovement();
    
    ofParameterGroup blobTrackerPms;

    searchZone searchZone;
    int getNumberOfActiveBlobs(); 
    ofxCv::ContourFinder* getContourFinderPointer();
    ofPixelsRef getPatchedImageCvPixelRef();
    ofPixelsRef getDrayDiffImagePixelRef();
    
private: 
    //CompVisionShizNit
    void drawContFinder();

    ofVec2f k; // kinect width and height
    ofxCv::ContourFinder    contFinder;
    ofxCvGrayscaleImage     patchedImageCv;
    ofxCvGrayscaleImage     grayDiffOfImage;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     bgImage;
    ofxCvGrayscaleImage     lastFrameGrayDiffImage;
    
    float   movementAmount;
    
    ofParameter<bool> bLearnBackground,bBackgroundLearned,bForgetBackground, bMovementTresholdOn;
    ofParameter<float> movementTreshold;
    ofParameter<int> minArea;

};
#endif /* defined(__ffortress__blobTracker__) */
