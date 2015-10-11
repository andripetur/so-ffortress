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

void videoProjection::draw(ofxCv::ContourFinder*
                           contourFinder){
    
    ofPushMatrix();
    ofBackground(0);
    ofTranslate(moveX,moveY);
    ofScale(1+(scale/50.0),1+(scale/50.0),1);
    
    int label;
    ofPolyline smoothPolyLine;
    ofTexture blockTexture; 
    for(int i = 0; i < contourFinder->size(); i++) {
        //fxCv::RectTracker& tracker = contourFinder.getTracker();
        // get contour, label, center point, and age of contour
        //vector<cv::Point> points = contourFinder.getContour(i);
        //ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
        
        label = contourFinder->getLabel(i);

        //contour smoothing
        smoothPolyLine = contourFinder->getPolyline(i).getSmoothed(10, 0.0);
        
        // get the right texture
        switch ( blockMap.find(label)->second ) {
            case RED_BLOCK:
                blockTexture = redBlock.getimage().getTextureReference();
                break;
                
            case BLUE_BLOCK:
                blockTexture = blueBlock.getimage().getTextureReference();
                break;
            
            case YELLOW_BLOCK:
                blockTexture = yellowBlock.getfbo().getTextureReference();
                break;
        }
        
        // resize and draw textures on bounding box
        texturedMesh.draw(  ofxCv::toOf( contourFinder->getBoundingRect(i) ),
                            blockTexture,
                            smoothPolyLine,
                            label);
    }
    
    if(bDrawContours) contourFinder->draw(); // draw contours for alignment
    ofPopMatrix();
    
}

void videoProjection::drawLabelColorMap(){
    stringstream labelClrs;
    
    //            pair<int, int> n;
    for( auto & n : blockMap){
        labelClrs << "Label: " << n.first << " has color: " << BlkTyToStngColor( n.second ) << endl;
    }
    
    ofDrawBitmapString(labelClrs.str(), ofPoint(ofGetScreenWidth() * 0.5, ofGetScreenHeight() * 0.75));
}

// color string to block type
int videoProjection::clrStngToBlkT(string clr){
    int numberated=0;
    
    if (clr == "red") {
        numberated = RED_BLOCK;
    } else if ( clr == "blue"){
        numberated = BLUE_BLOCK;
    } else if ( clr == "yellow"){
        numberated = YELLOW_BLOCK;
    } else if ( clr == "black"){
        numberated = BLACK_BLOCK;
    } else if ( clr == "white"){
        numberated = WHITE_BLOCK;
    }
    
    return numberated; 
}

// block type to string color
string videoProjection::BlkTyToStngColor(int blT){
    string clr;
    
    if (blT == RED_BLOCK) {
        clr = "red";
    } else if ( blT == BLUE_BLOCK){
        clr = "blue";
    } else if ( blT == YELLOW_BLOCK){
        clr = "yellow";
    } else if ( blT == BLACK_BLOCK){
        clr = "black";
    } else if ( blT == WHITE_BLOCK){
        clr = "white";
    }
    
    return clr;
}

void videoProjection::setupParameters(){
    
    // setup guis
    videoParameters.setName("Video Parameters");
    videoParameters.add(scale.set("Scale", 1.0, 0.5, 200.0));
    videoParameters.add(moveX.set("Move X", 0, -1000, 1000));
    videoParameters.add(moveY.set("Move Y", 0, -1000, 1000));
    videoParameters.add(bDrawContours.set("drawContours", false)); 
    
    redBlockParameters.setName("Red Block");
    redBlockParameters.add(brightness.set("Brightness", 70, 0, 1000));
    redBlockParameters.add(speed.set("Speed", 150, 0, 300));
    
}