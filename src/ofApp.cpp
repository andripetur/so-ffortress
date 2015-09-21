#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    kinect.setup();
    clrNamer.setup();
    cout << clrNamer.numCols << endl;
    
    ofSetLogLevel(OF_LOG_SILENT);
    
    // allocate space for compVis shiznit
    grayImage.allocate(kinect.width, kinect.height);
    grayDiff.allocate(kinect.width, kinect.height);
    grayDiffOfImage.allocate(kinect.width, kinect.height);
    bgImage.allocate(kinect.width, kinect.height);
    bLearnBackground = false;
    bBackgroundLearned = false;
    bForgetBackground = false;
    
    bShowLabels = true;
    
    contFinder.setMinArea(100);
    contFinder.setMaxArea((kinect.width*kinect.height)/2);
    contFinder.setThreshold(5);
//    contFinder.setFindHoles(false);

    // wait for a frame before forgetting something
    contFinder.getTracker().setPersistence(30);
    // an object can move up to 32 pixels per frame
    contFinder.getTracker().setMaximumDistance(32);
    
	ofSetFrameRate(60);
    // open an outgoing OSC connection to HOST:PORT
    sender.setup(HOST, PORT);
    
    setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    gui.add(kinect.patchCloudParameters);
    gui.add(kinect.viewPointCloudParameters);
    gui.add(bShowInfo.set("info", false));
    gui.add(minArea.set("cvMinArea", 100, 50, 300));
    gui.setPosition(820, 10);
    gui.saveToFile("patchCloudParameters");
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();
    
    if(kinect.getHasNewFrame()){
        grayImage = kinect.getPatchedCvImage(); // get the merged cvImage from the two kinects
        
        // set new background image
        if(bLearnBackground){
            bgImage = grayImage;   // let this frame be the background image from now on
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
        
        grayImage.flagImageChanged();
        if(bBackgroundLearned){
            cvAbsDiff(bgImage.getCvImage(), grayImage.getCvImage(), grayDiff.getCvImage());
            cvErode(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 2);
            cvDilate(grayDiff.getCvImage(), grayDiff.getCvImage(), NULL, 1);
            // threshold ignoring little differences
            cvThreshold(grayDiff.getCvImage(), grayDiff.getCvImage(), 4, 255, CV_THRESH_BINARY);
            grayDiff.flagImageChanged();
            // update the ofImage to be used as background mask for the blob finder
            grayDiffOfImage.setFromPixels(grayDiff.getPixels(), kinect.width, kinect.height);
            
            // update the cv images
            grayDiffOfImage.flagImageChanged();

            // pass image on to contour finder
            contFinder.findContours(grayDiffOfImage.getCvImage());
        } else {
            contFinder.findContours(grayImage.getCvImage());
        }//backGroundLearned
    }
    
    oscSender();
}

//--------------------------------------------------------------
void ofApp::oscSender(){
//#define SHOW_SENDER_DEBUG
    ofxCv::RectTracker& tracker = contFinder.getTracker();
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
    
    ofPoint loc;
    ofRectangle area;
    int label;
    ofPixelsRef patchedImagePixRef = patchedImageCv.getPixelsRef();
    
    // new block = /newBlob <label> <color>
    for(int i=0; i< newLabels.size(); ++i){
        ofxOscMessage m;
        m.setAddress("/newBlob");
        m.addIntArg(newLabels[i]);      // label
        m.addStringArg("/color");       // color
        sender.sendMessage(m);
    }
    
    // block update = /currentBlob <label> <area> <age> <x> <y> <z>
    for(int i=0; i<contFinder.size(); ++i){
        ofxOscMessage m;
        m.setAddress("/currentBlob");
        m.addIntArg((label = contFinder.getLabel(i)) );        // label
        area = ofxCv::toOf( contFinder.getBoundingRect(i));
        m.addIntArg((area.width*area.height ));                // area
        m.addIntArg(tracker.getAge(label) );                   // age
        loc = ofxCv::toOf(contFinder.getCenter(i));
        m.addIntArg(loc.x);                                    // x
        m.addIntArg(loc.y);                                    // y
        m.addIntArg( patchedImagePixRef.getColor(loc.x, loc.y).getBrightness()); // z
        sender.sendMessage(m);
    }
    
    // block dead = /deadBlob <label>
    for (int i=0; i<deadLabels.size(); ++i) {
        ofxOscMessage m;
        m.setAddress("/deadBlob");
        m.addIntArg((label = deadLabels[i] ));        // label
        sender.sendMessage(m);
    }
#ifdef SHOW_SENDER_DEBUG
    cout << "new labels:     " << newLabels.size() << endl;
    cout << "current labels: " << contFinder.size() << endl;
    cout << "dead labels:    " << deadLabels.size() << endl<<endl;
#endif
}

//--------------------------------------------------------------
ofColor ofApp::avgColor(ofRectangle area, float offsetRatio){
    ofColor avgColor, currColor;
    ofPixelsRef greyDiffPix = grayDiffOfImage.getPixelsRef();
    unsigned int r, g, b;
    r = g = b = 0;
    int numberOfColoredPixels = 0;
    int horisontalOffset = int(area.width*offsetRatio);
    int verticalOffset = int(area.height*offsetRatio);
    int stepSize = 5;
    
    for(int x=horisontalOffset; x<area.width-horisontalOffset; x+=stepSize){
        for(int y=verticalOffset; y<area.height-verticalOffset; y+=stepSize){
            
            if(greyDiffPix.getColor(x, y) != ofColor(0,0,0)){ //only check color of nonblack areas in the blob
                currColor = kinect.getColorAt(area.x + x, area.y + y);
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
    } else {
        avgColor = ofColor(r, g, b);
    }
    return avgColor;
}

//--------------------------------------------------------------
void ofApp::drawContFinder(){
    ofSetBackgroundAuto(bShowLabels);
    ofxCv::RectTracker& tracker = contFinder.getTracker();
    
    if(bShowLabels) {
        ofSetColor(255);
        if(bBackgroundLearned){
            grayDiffOfImage.draw(0,0);
        } else {
            grayImage.draw(0, 0);
        }
        contFinder.draw();
        for(int i = 0; i < contFinder.size(); ++i) {
            ofPoint center = ofxCv::toOf(contFinder.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contFinder.getLabel(i);
            string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            ofDrawBitmapString(msg, 0, 0);
            ofVec2f velocity = ofxCv::toOf(contFinder.getVelocity(i));
            ofScale(5, 5);
            ofLine(0, 0, velocity.x, velocity.y);
            ofPopMatrix();
        }
    } else {
        for(int i = 0; i < contFinder.size(); ++i) {
            unsigned int label = contFinder.getLabel(i);
            // only draw a line if this is not a new label
            if(tracker.existsPrevious(label)) {
                // use the label to pick a random color
                ofSeedRandom(label << 24);
                ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
                // get the tracked object (cv::Rect) at current and previous position
                const cv::Rect& previous = tracker.getPrevious(label);
                const cv::Rect& current = tracker.getCurrent(label);
                // get the centers of the rectangles
                ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
                ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
                ofLine(previousPosition, currentPosition);
            }
        }
    }
    
    // this chunk of code visualizes the creation and destruction of labels
    const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
    const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
    ofSetColor(ofxCv::cyanPrint);
    int j;
    for(int i = 0; i < currentLabels.size(); ++i) {
        j = currentLabels[i];
        ofLine(j, 0, j, 4);
    }
    ofSetColor(ofxCv::magentaPrint);
    for(int i = 0; i < previousLabels.size(); ++i) {
        j = previousLabels[i];
        ofLine(j, 4, j, 8);
    }
    ofSetColor(ofxCv::yellowPrint);
    for(int i = 0; i < newLabels.size(); ++i) {
        j = newLabels[i];
        ofLine(j, 8, j, 12);
    }
    ofSetColor(ofColor::white);
    for(int i = 0; i < deadLabels.size(); ++i) {
        j = deadLabels[i];
        ofLine(j, 12, j, 16);
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
	ofBackground(100, 100, 100);
	ofSetColor(255, 255, 255);
	
    kinect.draw();
	if(!kinect.isPointCloudDrawn()) {
        
        ofPushMatrix();
            ofTranslate(420, 10);
            ofScale(0.625, 0.625);
            drawContFinder();
        ofPopMatrix();
        
//        // Find color of blob and draw it
//        ofRectangle boundingRect;
//        ofColor temp;
//        if( contFinder.size() > 0) {
//            for(int i=0; i<contFinder.size(); ++i){
//                boundingRect = ofxCv::toOf(contFinder.getBoundingRect(i));
//                if(boundingRect.getCenter().y > kinect.height * 0.5){
//                    ofSetColor( (temp = avgColor(boundingRect, 0 )) );
//                    ofRect(420, 320, 400, 300);
////                    cout
////                    << "r: " << (int)temp.r
////                    << " b: " << (int)temp.b
////                    << " g: " << (int)temp.g << endl;
//                }
////                ofDrawBitmapString(colorNamer(temp), 420, 320); // draw name of color
//            }
//        }
        
        ofSetColor(testColor);
        ofRect(420, 320, 400, 300);
//        ofDrawBitmapString(colorNamer(testColor), 420, 300); // draw name of color
        if(bColorChanged){
        cout << "color name: "
        << " R: " << (int)testColor.r
        << " B: " << (int)testColor.b
        << " G: " << (int)testColor.g << endl;
            bColorChanged = false;
        }
        
	}

	// draw instructions
    if(bShowInfo){
        ofSetColor(255, 255, 255);
        stringstream reportStream;
    
        reportStream
        << " b to learn background."<< endl
        << " f to forget background."<< endl
        << " space to dis/enable mouse input for pointcloud"<< endl
        << " num blobs found " << contFinder.size()
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
            
        case'l':
        case'L':
            bShowLabels = !bShowLabels;
            break;
            
        case'r':
        case'R':
            testColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
            bColorChanged = true;
            break;
	}
}