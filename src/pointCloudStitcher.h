//
//  pointCloudStitcher.h
//  kinectExample
//
//  Created by Andri Pétur Þrastarson on 17/09/15.
//
//

#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofApp.h"

#ifndef __kinectExample__pointCloudStitcher__
#define __kinectExample__pointCloudStitcher__

#define amt 2

class pointCloudStitcher{
public:
    pointCloudStitcher(){
        ofAddListener(ofEvents().exit, this, &pointCloudStitcher::exit);
        ofAddListener(ofEvents().keyPressed, this, &pointCloudStitcher::keyListener);
    }
    
    void setup();
    void update();
    void draw();
    void exit(ofEventArgs & a);
    void keyListener(ofKeyEventArgs & a);
    
    ofParameterGroup patchCloudParameters, viewPointCloudParameters;
    
    int width;
    int height;
    bool isConnected();
    bool isPointCloudDrawn();
    bool getHasNewFrame(); 
    
    ofxCvGrayscaleImage getPatchedCvImage();
    
private:
    ofxKinect kinect[amt];
    int angle;
    bool hasNewFrame;
    ofxCvGrayscaleImage grayImage[amt]; // grayscale depth image
    ofxCvGrayscaleImage patchedImageCv;
    
    // To view and adjust point cloud
    bool bDrawPointCloud;
    ofMesh mesh;
    ofEasyCam cam;

    ofParameter<float> mapFromLowZ, mapFromHighZ, mapToLowZ, mapToHighZ, transformConstant, pointSize;
    ofParameter<int> kinectDistance, kinectDistanceY, stepSize, pointCloudFarClipping;
    ofParameter<bool> bEnableMouseInput, bShowInfo;
    
    void drawPointCloud();
    void setupParameters();
    
    float** adjustPointClouds(ofxKinect* kPtrO,ofxKinect* kPtrT);
    void mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float ** adj);
    
};

#endif /* defined(__kinectExample__pointCloudStitcher__) */
