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

#define N_FRAMES 3

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
    
    void setFrameSmoothing(bool use);
    ofColor getColorAt(int x, int y);
    ofColor getPatchedColorAt(int x, int y);
    
    ofxCvGrayscaleImage getPatchedCvImage();
    ofxCvColorImage getPatcheColorImage();
    
private:
    ofxKinect kinect[amt];
    int angle;
    bool hasNewFrame;
    ofxCvGrayscaleImage grayImage[amt]; // grayscale depth image
    ofxCvGrayscaleImage patchedImageCv, pathcedImgCvSmooth;
    ofxCvColorImage colorImage[amt];
    ofxCvColorImage patchedImageColor,patchedImageClrSmooth;
    
    ofPixels clrAdjust(ofxCvColorImage *img);
    float sampleBrightness(ofPixels imgPix, int steps);

    // To view and adjust point cloud
    bool bDrawPointCloud;
    ofMesh mesh;
    ofEasyCam cam;

    void setupParameters();
    ofParameter<float> mapFromLowZ, mapFromHighZ, mapToLowZ, mapToHighZ, transformConstant, pointSize;
    ofParameter<int> kinectDistance, kinectDistanceY, stepSize, pointCloudFarClipping;
    ofParameter<bool> bEnableMouseInput, bShowInfo, bUseClrAdjustment;
    
    void drawPointCloud();
    
    // frame smoothing
    unsigned int frameCount;
    ofParameter<bool> bUseFrameSmoothing;
    bool bNewAccumedFrame;
    ofxCvGrayscaleImage frames[N_FRAMES+1];
    ofxCvColorImage clrFrames[N_FRAMES+1]; 
    void frameSmoother();
    
    float** adjustPointClouds(ofxKinect* kPtrO,ofxKinect* kPtrT);
    void mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float **adj);
    void mergeColorImages(ofxCvColorImage imgOne, ofxCvColorImage imgTwo, float **adj);
    ofPixelsRef merge(ofPixelsRef imgOnePix, ofPixelsRef imgTwoPix, float **adj, int width, int height);
    ofColor lastColor; 
    
};

#endif /* defined(__kinectExample__pointCloudStitcher__) */
