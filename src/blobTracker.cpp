//
//  blobTracker.cpp
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 11/10/15.
//
//

#include "blobTracker.h"

void blobTracker::setup(ofVec2f k){
    this->k = k;
    
    // allocate space for compVis shiznit
    patchedImageCv.allocate(k.x, k.y);
    grayDiff.allocate(k.x, k.y);
    grayDiffOfImage.allocate(k.x, k.y);
    bgImage.allocate(k.x, k.y);
    lastFrameGrayDiffImage.allocate(k.x, k.y);
    
    // set contFinderVariables
    contFinder.setMinArea(100);
    contFinder.setMaxArea((k.x * k.y)/2);
    contFinder.setThreshold(5);
    contFinder.setFindHoles(false);
    
    // wait for a frame before forgetting something
    contFinder.getTracker().setPersistence(30);
    // an object can move up to 32 pixels per frame
    contFinder.getTracker().setMaximumDistance(32);
    
    // setupSearchZOne
    searchZone.setup(k);
    movementAmount = 0;

    setupParameters();
}

void blobTracker::setupParameters(){
    blobTrackerPms.setName("blobTracker");
    // init bg extract to off
    blobTrackerPms.add(bLearnBackground.set("learn background", false));
    bBackgroundLearned = false;
    bForgetBackground = true;
    blobTrackerPms.add(minArea.set("cvMinArea", 100, 50, 300));
    blobTrackerPms.add(bMovementTresholdOn.set("movementTresholdOn", false));
    blobTrackerPms.add(movementTreshold.set("movementTreshold", 0.5, 0, 1));
}

void blobTracker::update(ofxCvGrayscaleImage depthImage){
    patchedImageCv = depthImage; // depth image is merged cvImage from the two kinects
    
    // set new background image
    if(bLearnBackground){
        bgImage = patchedImageCv;   // let this frame be the background image from now on
        bLearnBackground = false;
        bBackgroundLearned = true;
    }
    
    // forget background image
    if(bForgetBackground){
        bBackgroundLearned = false;
        bForgetBackground = false;
    }
    // set minimal blob area
    contFinder.setMinArea(minArea);
    
    patchedImageCv.flagImageChanged();
    if(bBackgroundLearned){
        cvAbsDiff(bgImage.getCvImage(), patchedImageCv.getCvImage(), grayDiff.getCvImage());
        cvErode(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 2);
        cvDilate(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 1);
        // threshold ignoring little differences
        cvThreshold(grayDiff.getCvImage(), grayDiff.getCvImage(), 4, 255, CV_THRESH_BINARY);
        grayDiff.flagImageChanged();
        // update the ofImage to be used as background mask for the blob finder
        grayDiffOfImage.setFromPixels(grayDiff.getPixels(), k.x, k.y);
        
        // update the cv images
        grayDiffOfImage.flagImageChanged();
        
        calculateMovement();
        
        // pass image on to contour finder
        // if moventTresholding is on and movement is less than thresold value
        // if movement thresholding is off
        if( (bMovementTresholdOn && (movementAmount < movementTreshold)) || bMovementTresholdOn == false) {
            contFinder.findContours(grayDiffOfImage.getCvImage());
        }
        
    } else {
        contFinder.findContours(patchedImageCv.getCvImage());
    }//backGroundLearned
}

void blobTracker::calculateMovement(){
    int currFrameMovement;
    int nrOfPixelsCounted;
    
    //check the difference beetween last and current frame
    lastFrameGrayDiffImage.absDiff(grayDiffOfImage);
    if( searchZone.isOn() ){
        ofPoint tpLeft = searchZone.getArea().getTopLeft();
        ofPoint btRight = searchZone.getArea().getBottomLeft();
        nrOfPixelsCounted = searchZone.getArea().getArea();
        
        currFrameMovement = lastFrameGrayDiffImage.countNonZeroInRegion(tpLeft.x, tpLeft.y, btRight.x, btRight.y);
    } else {
        nrOfPixelsCounted = (k.x*k.y);

        currFrameMovement = lastFrameGrayDiffImage.countNonZeroInRegion(0, 0, k.x, k.y);
    }
    
    // normalize value
    currFrameMovement /= (float)nrOfPixelsCounted;
    
    // Smooth value
    movementAmount = (movementAmount * 7 + currFrameMovement + 7) / (float)8;
    
    cout << "momvement amt: "<< movementAmount << endl;

    lastFrameGrayDiffImage = grayDiffOfImage;
}

void blobTracker::drawContourFinder(){
    ofxCv::RectTracker& tracker = contFinder.getTracker();
    
    // draw merged depth image.
    if(bBackgroundLearned){
        // draw bg extraction
        grayDiffOfImage.draw(0,0);
    } else {
        // draw clean
        patchedImageCv.draw(0, 0);
    }
    
    // draw contour finder bw image
    contFinder.draw();
    
    // draw search rectangle
    searchZone.draw();
    
    // draw label and age.
    int label;
    string msg;
    for(int i = 0; i < contFinder.size(); ++i) {
        ofPushMatrix();
        ofTranslate( ofxCv::toOf(contFinder.getCenter(i)) );
        label = contFinder.getLabel(i);
        ofSetColor( 0, 255, 50);
        msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
        ofDrawBitmapString(msg, 0, 0);
        ofPopMatrix();
    }
}

void blobTracker::draw(){
    ofPushMatrix();
        ofTranslate(420, 10);
        ofScale(0.625, 0.625);
        drawContourFinder();
    ofPopMatrix();
}

void blobTracker::keyListener(ofKeyEventArgs & a){
    switch(a.key){
        case 'b':
            bLearnBackground = true;
            break;
            
        case 'f':
            bForgetBackground = true;
            break;
    }
}

ofxCv::ContourFinder* blobTracker::getContourFinderPointer(){
    return &contFinder;
}
ofPixelsRef blobTracker::getPatchedImageCvPixelRef(){
    return patchedImageCv.getPixelsRef();
}

ofPixelsRef blobTracker::getDrayDiffImagePixelRef(){
    return grayDiffOfImage.getPixelsRef();
}
int blobTracker::getNumberOfActiveBlobs(){
    return contFinder.size();
}