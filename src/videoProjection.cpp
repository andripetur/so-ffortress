//
//  videoProjection.cpp
//  Fortress_mapping
//
//  Created by Thijs Muijs on 10/9/15.
//
//

#include "videoProjection.h"


void videoProjection::setup(){
    
    W = H = 400;
    redBlock.setup(W,H);
    blueBlock.setup(W,H);
    greenBlock.setup(W,H);
    yellowBlock.setup(200,200);
    
    texturedMesh.setup();
    setupParameters();
    
}

void videoProjection::update(){
    
    redBlock.update(0,0,brightness,speed);
    blueBlock.update(0, 0, 25, 25, 100, ofGetFrameNum());
    greenBlock.update(ofGetFrameNum());
    yellowBlock.update(1.0, ofGetFrameNum());
    texturedMesh.update();
}

void videoProjection::draw(ofxCv::ContourFinder &contourFinder){
    
    ofBackground(0);
    
    ofPushMatrix();
    ofTranslate(moveX,moveY);
    ofScale(1+(scale/50.0),1+(scale/50.0),1);
    
    for(int i = 0; i < contourFinder.size(); i++) {
        //fxCv::RectTracker& tracker = contourFinder.getTracker();
        // get contour, label, center point, and age of contour
        //vector<cv::Point> points = contourFinder.getContour(i);
        //ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
        
            int label = contourFinder.getLabel(i);
                // int age = tracker.getAge(label);

        //contour smoothing
        ofPolyline smoothPolyline = contourFinder.getPolyline(i).getSmoothed(10, 0.0);
        
        if(i < 1){
            // resize and draw textures on bounding box
            texturedMesh.draw(contourFinder.getBoundingRect(i).x, contourFinder.getBoundingRect(i).y, contourFinder.getBoundingRect(i).width, contourFinder.getBoundingRect(i).height, redBlock.getimage().getTextureReference(), smoothPolyline, label);
        }
        else{
            texturedMesh.draw(contourFinder.getBoundingRect(i).x, contourFinder.getBoundingRect(i).y, contourFinder.getBoundingRect(i).width, contourFinder.getBoundingRect(i).height, yellowBlock.getfbo().getTextureReference(), smoothPolyline, label);
        }
    }
    ofPopMatrix();
    
}



void videoProjection::setupParameters(){
    
// setup guis
videoParameters.setName("Video Parameters");
videoParameters.add(scale.set("Scale", 1.0, 0.5, 200.0));
videoParameters.add(moveX.set("Move X", 0, -1000, 1000));
videoParameters.add(moveY.set("Move Y", 0, -1000, 1000));
    
    RedBlock.setName("Red Block");
    RedBlock.add(brightness.set("Brightness", 70, 0, 1000));
    RedBlock.add(speed.set("Speed", 150, 0, 300));
    
}