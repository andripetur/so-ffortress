#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    kinect.setup();
    
    grayImage.allocate(kinect.width, kinect.height);
    grayDiff.allocate(kinect.width, kinect.height);
    grayDiffOfImage.allocate(kinect.width, kinect.height);
    bgImage.allocate(kinect.width, kinect.height);
    bLearnBackground = false;
    bBackgroundLearned = false;
    bForgetBackground = false;
    
	ofSetFrameRate(60);
        
    // open an outgoing OSC connection to HOST:PORT
    sender.setup(HOST, PORT);
        
    setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    gui.add(kinect.patchCloudParameters);
    gui.add(bShowInfo.set("info", false));
    gui.setPosition(820, 10);
    gui.saveToFile("patchCloudParameters");
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();
    
    // set new background image
    if(bLearnBackground){
        bgImage = kinect.getPatchedCvImage();   // let this frame be the background image from now on
        bLearnBackground = false;
        bBackgroundLearned = true;
    }
    
    if(bForgetBackground){
        bBackgroundLearned = false;
        bForgetBackground = false;
    }
    
    if(bBackgroundLearned){
        grayImage = kinect.getPatchedCvImage();
        cvAbsDiff(bgImage.getCvImage(), grayImage.getCvImage(), grayDiff.getCvImage());
        cvErode(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 3);
        cvDilate(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 1);
        // threshold ignoring little differences
        cvThreshold(grayDiff.getCvImage(), grayDiff.getCvImage(), 20, 255, CV_THRESH_BINARY);
        grayDiff.flagImageChanged();
        // update the ofImage to be used as background mask for the blob finder
        grayDiffOfImage.setFromPixels(grayDiff.getPixels(), kinect.width, kinect.height);
        
        // update the cv images
        grayDiffOfImage.flagImageChanged();

        // find contours which are between the size of 100 pixels and 1/3 the w*h pixels.
        contourFinder.findContours(grayDiffOfImage, 100, (kinect.width*kinect.height)/2, 20, false);
    }//isFrameNew

    /*
    // send a osc message for every blob
    // format /blobs <id> <area> <x> <y> <z>
    int nrOfBlobs = 1;
    if( contourFinder.blobs.size() > 0) {
        for(vector<ofxCvBlob>::iterator it = contourFinder.blobs.begin(); it != contourFinder.blobs.end(); ++it) {
            ofxOscMessage m;
            m.setAddress("/blobs");
            m.addIntArg(nrOfBlobs);
            m.addIntArg(it->area);
            //location
            m.addIntArg(it->centroid.x);
            m.addIntArg(it->centroid.y);
            m.addIntArg(it->centroid.z);
            sender.sendMessage(m);
            cout << "message sent to blob: " << nrOfBlobs << endl;
            ++nrOfBlobs;
        } //for
    } else {
        ofxOscMessage m;
        m.setAddress("/blobs");
        for(int i = 0; i<5;++i){
            m.addIntArg(0); // send to all poly instances, all info set to zero
        }
        sender.sendMessage(m);
        
    }// if
     */
}
/*
//--------------------------------------------------------------
ofColor ofApp::avgColor(ofRectangle area, float offsetRatio){
    ofColor avgColor, currColor;
    ofPixelsRef greyDiffPix = grayDiffOfImage[0].getPixelsRef();
    unsigned int r, g, b;
    r = g = b = 0;
    int numberOfColoredPixels = 0;
    int horisontalOffset = int(area.width*offsetRatio);
    int verticalOffset = int(area.height*offsetRatio);
    int stepSize = 5;
    
    for(int x=horisontalOffset; x<area.width-horisontalOffset; x+=stepSize){
        for(int y=verticalOffset; y<area.height-verticalOffset; y+=stepSize){
            
            if(greyDiffPix.getColor(x, y) != ofColor(0,0,0)){ //only check color of nonblack areas in the blob
//                currColor = kinect[0].getColorAt(area.x + x, area.y + y);
                r+= currColor.r;
                g+= currColor.g;
                b+= currColor.b;
                ++numberOfColoredPixels;
            }
        }
    }
    if(numberOfColoredPixels != 0){
        avgColor = ofColor(r/(float)numberOfColoredPixels,
                           g/(float)numberOfColoredPixels,
                           b/(float)numberOfColoredPixels);
    }
    return avgColor;
}
 */

//--------------------------------------------------------------
string ofApp::colorNamer(ofColor tBn){
    int r = tBn.r;
    int g = tBn.g;
    int b = tBn.b;
    string colorName;
    
    if(tBn == ofColor(0)){ // check if black
        colorName = "black";
    } else if( tBn == ofColor(255) ){ // check if white
        colorName = "white";
    } else {
        if(r == g && r == b){       // if r,g,b are same = grey
            colorName = "gray";
        } else {
            int grens = 20;
            if(r > b) {
                if(abs(r-g) < grens){
                    colorName = "yellow";
                } else if( r > g ){
                    colorName = "red";
                }
                if(abs(b - r) < grens){
                    colorName == "Violet";
                }
            }
            
            if( g > r ){
                if(abs(g - b) < grens) {
                    if( g > 56 ){
                        colorName = "blue";
                    } else {
                        colorName = "green";
                    }
                } else if(g > b) {
                    colorName = "green";
                }
            }
            
            if( b>g ){
                if(abs(b - r) < grens){
                    colorName == "Violet";
                } else if( b>r ){
                    colorName = "blue";
                }
            }
            
        }

    }
    
    return colorName;
}

//--------------------------------------------------------------
void ofApp::draw() {
    
	ofBackground(100, 100, 100);
	ofSetColor(255, 255, 255);
	
    kinect.draw();
	if(!kinect.isPointCloudDrawn()) {
        
        grayDiffOfImage.draw(10, 320, 200, 150);
		contourFinder.draw(10, 320, 200, 150);
        
        if(bBackgroundLearned){
            grayDiffOfImage.draw(420, 10, 400, 300);
            contourFinder.draw(420, 10, 400, 300);
        } else {
            kinect.getPatchedCvImage().draw(420, 10, 400, 300);
        }
        /*
        // Find color of blob and draw it
        if( contourFinder.blobs.size() > 0) {
            for(vector<ofxCvBlob>::iterator it = contourFinder.blobs.begin(); it != contourFinder.blobs.end(); ++it){
                ofSetColor( avgColor(it->boundingRect, 0.1f ) );
                ofRect(420, 320, 400, 300);
//                ofDrawBitmapString(colorNamer(temp), 420, 320); // draw name of color
            }
        }
         */
     
    
        /*
        if( ofGetElapsedTimeMillis() - timer >= 2000 ){
            timer = ofGetElapsedTimeMillis();
            testColor = ofColor( ofRandom(255), ofRandom(255), ofRandom(255));
        }
        
        ofSetColor(testColor);
        ofRect(420, 320, 400, 300);
        ofDrawBitmapString(colorNamer(testColor), 420, 320); // draw name of color
         */
	}

	// draw instructions
    if(bShowInfo){
        ofSetColor(255, 255, 255);
        stringstream reportStream;
    
        reportStream
        << " b to learn background."<< endl
        << " f to forget background."<< endl
        << " space to dis/enable mouse input"<< endl
        << " num blobs found " << contourFinder.nBlobs
        << " fps: " << ofGetFrameRate() << endl
        << " c to close connection, o to open it again, connection is: " << kinect.isConnected() << endl;
    
        ofDrawBitmapString(reportStream.str(), 20, ofGetWindowHeight()*0.75);
    }
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::exit() {

}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
        case 'b':
            bLearnBackground = true;
            break;
            
        case 'f':
            bForgetBackground = true;
            break;
            
        case'i':
        case'I':
            bShowInfo = !bShowInfo;
            break;
	}
}