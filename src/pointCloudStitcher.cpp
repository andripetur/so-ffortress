//
//  pointCloudStitcher.cpp
//  kinectExample
//
//  Created by Andri Pétur Þrastarson on 17/09/15.
//
//

#include "pointCloudStitcher.h"

void pointCloudStitcher::setup(){
    // zero the tilt on startup
    angle = 10;
    
    for(int i=0; i<amt; ++i){
        kinect[i].setRegistration(true);
        kinect[i].init(false, true);
        kinect[i].open();
        kinect[i].setCameraTiltAngle(angle);
        
        grayImage[i].allocate(kinect[0].width, kinect[0].height);
    }
    width = kinect[0].width;
    height = kinect[0].height;
    patchedImageCv.allocate(kinect[0].width, kinect[0].height);
    
    // start from the front
    bDrawPointCloud = false;
    setupParameters();
}

void pointCloudStitcher::setupParameters(){
    patchCloudParameters.setName("PatchCloudParameters");
    patchCloudParameters.add(mapFromLowZ.set("mapFromLowZ", 0, 0, 1000));
    patchCloudParameters.add(mapFromHighZ.set("mapFromHighZ", 255, 255, 6500));
    patchCloudParameters.add(mapToLowZ.set("mapToLowZ", 0, -500, 100));
    patchCloudParameters.add(mapToHighZ.set("mapToHighZ", 0, -800, 500));
    patchCloudParameters.add(transformConstant.set("transformConstant", 1, 0, 10));
    patchCloudParameters.add(kinectDistance.set("Kinect Distance", 0, 0, 1000));
    patchCloudParameters.add(kinectDistanceY.set("Kinect Distance Y", 0, -100, 100));
    patchCloudParameters.add(pointCloudFarClipping.set("P.C. Far Clip", 0, 0, 10000));
    patchCloudParameters.add(stepSize.set("stepSize", 4, 1, 10));
    patchCloudParameters.add(pointSize.set("pointSize", 4, 1, 10));
    patchCloudParameters.add(bEnableMouseInput.set("Mouse Input", true));
}

void pointCloudStitcher::update(){
    if(bDrawPointCloud){
        cam.setFarClip(pointCloudFarClipping);
        if(bEnableMouseInput){
            cam.enableMouseInput();
        } else {
            cam.disableMouseInput();
        }
    }
    for(int i=0; i<amt;++i) kinect[i].update();
    
    // there is a new frame and we are connected
    if(kinect[0].isFrameNew()) {
        
        for(int i=0;i<amt;++i){
            // load grayscale depth image from the kinect source
            grayImage[i].setFromPixels(kinect[i].getDepthPixels(), kinect[i].width, kinect[i].height);
            
        }
        
    }
    float ** adj;
    adj = adjustPointClouds(&kinect[0], &kinect[1]);
    
    mergeGrayImages(grayImage[0], grayImage[1], adj);

}

void pointCloudStitcher::draw(){
    if(bDrawPointCloud){
        cam.begin();
            drawPointCloud();
        cam.end();
    } else {
        // draw from the live kinect
        kinect[0].drawDepth(10, 10, 200, 150);
        kinect[1].drawDepth(210, 10, 200, 150);
        
        kinect[0].draw(10, 160, 200, 150); //rgb
        kinect[1].draw(210, 160, 200, 150); //rgb
    }
}

float** pointCloudStitcher::adjustPointClouds(ofxKinect* kinectOnePtr, ofxKinect* kinectTwoPtr){
    static bool bFirstTime = true;
    int step;
    if(bDrawPointCloud){
        step = stepSize;
    } else {
        step = 1;
    }
    int distanceZ;
    static float ** adjustments;
    if(bFirstTime){
        adjustments = new float *[kinectOnePtr->width];
        for(int i = 0; i<kinectOnePtr->width; ++i){
            adjustments[i] = new float [kinectOnePtr->height];
        }
        
        for(int y = 0; y < kinectOnePtr->height; y += step) {
            for(int x = 0; x < kinectOnePtr->width; x += step) {
                adjustments[x][y] = 1;
            }
        }
        bFirstTime = false;
    }
    
    ofPoint tPoCur, tp;
    
    mesh.clear();
    if(bDrawPointCloud){
        for(int y = 0; y < kinectOnePtr->height; y += step) {
            for(int x = 0; x < kinectOnePtr->width; x += step) {
                distanceZ =kinectOnePtr->getDistanceAt(x, y);
                if(distanceZ > 0) {
                    tPoCur = kinectOnePtr->getWorldCoordinateAt(x, y);
                    mesh.addColor( ofColor::red );
                    mesh.addVertex(tPoCur);
                }
            }
        }
    }
    
    float beforeAdjustment;
    for(int y = 0; y < kinectOnePtr->height; y += step) {
        for(int x = 0; x < kinectOnePtr->width; x += step) {
            distanceZ =kinectTwoPtr->getDistanceAt(x, y);
            if(distanceZ > 0) {
                tPoCur = kinectTwoPtr->getWorldCoordinateAt(x, y);
                beforeAdjustment = tPoCur.x;
                tPoCur.x+= ofMap(distanceZ, mapFromLowZ, mapFromHighZ, mapToLowZ, mapToHighZ)*transformConstant + kinectDistance;
                adjustments[x][y] = tPoCur.x/beforeAdjustment;
                tPoCur.y+= kinectDistanceY;
                if(bDrawPointCloud){
                    mesh.addColor( ofColor::blue );
                    mesh.addVertex(tPoCur);
                }
            } else {
                adjustments[x][y] = 1;
            }
        }
    }
    
    return adjustments;
}

void pointCloudStitcher::mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float ** adj){
    int brightnessOne;
    ofPoint tempPointCurrent;
    ofPixelsRef imgOnePix = imgOne.getPixelsRef();
    ofPixelsRef imgTwoPix = imgTwo.getPixelsRef();
    int offset;
    int brightness;
    for(int y = 0; y < imgTwo.getHeight(); ++y) {
        for(int x = 0; x < imgTwo.getWidth(); ++x) {
            brightnessOne = imgTwoPix.getColor(x, y).getBrightness();
            if(brightnessOne> 0) {
                offset = int(x * adj[x][y]);
                if(offset < imgOne.getWidth() && offset > 0) {
                    if( (brightness = imgOnePix.getColor(offset, y+kinectDistanceY).r) < brightnessOne ){
                        imgOnePix.setColor(offset, y+kinectDistanceY, imgTwoPix.getColor(x, y));
                    }
                }
                
            }
        }
    }
    
    patchedImageCv.setFromPixels(imgOnePix.getPixels(), imgOne.width, imgOne.height);
    patchedImageCv.flagImageChanged();
}

//--------------------------------------------------------------
void pointCloudStitcher::drawPointCloud() {
    glPointSize(pointSize);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards'
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    ofEnableDepthTest();
    mesh.drawVertices();
    ofDisableDepthTest();
    ofPopMatrix();
}

void pointCloudStitcher::exit(ofEventArgs & a){
    for(int i=0; i<amt; ++i){
        kinect[i].setCameraTiltAngle(0); // zero the tilt on exit
        kinect[i].close();
    }
}

void pointCloudStitcher::keyListener(ofKeyEventArgs & a){
    int key = a.key;
    switch (key) {
        case ' ':
            bEnableMouseInput = !bEnableMouseInput;
            break;
            
        case'p':
            bDrawPointCloud = !bDrawPointCloud;
            break;
            
        case 'o':
            for(int i=0;i<amt;++i) {
                kinect[i].setCameraTiltAngle(angle); // go back to prev tilt
                kinect[i].open();
            }
            break;
            
        case 'c':
            for(int i=0;i<amt;++i) {
                kinect[i].setCameraTiltAngle(0); // zero the tilt
                kinect[i].close();
            }
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            for(int i=0;i<amt;++i) kinect[i].setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            for(int i=0;i<amt;++i) kinect[i].setCameraTiltAngle(angle);
            break;
    }
}

bool pointCloudStitcher::isConnected(){
    return kinect[0].isConnected();
}
bool pointCloudStitcher::isPointCloudDrawn(){
    return bDrawPointCloud;
}
ofxCvGrayscaleImage pointCloudStitcher::getPatchedCvImage(){
    return patchedImageCv;
}