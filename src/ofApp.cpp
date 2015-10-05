#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    kinect.setup();
    clrNamer.setup();
    
    ofSetLogLevel(OF_LOG_SILENT);
    
    // allocate space for compVis shiznit
    patchedImageCv.allocate(kinect.width, kinect.height);
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
    contFinder.setFindHoles(false);

    // wait for a frame before forgetting something
    contFinder.getTracker().setPersistence(30);
    // an object can move up to 32 pixels per frame
    contFinder.getTracker().setMaximumDistance(32);
    
	ofSetFrameRate(10);
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
    gui.add(host.set("host", HOST));
    
    searchZonePmGroup.setName("SearchZonePms");
    searchZonePmGroup.add(bSearchZoneOn.set("SearchZoneEnable", false));
    searchZonePmGroup.add(sZwidth.set("width", 100, 2, kinect.width));
    searchZonePmGroup.add(sZheight.set("height", 100, 2, kinect.height));
    searchZonePmGroup.add(szCenter.set("center", ofPoint(kinect.width*0.5, kinect.height*0.5), ofPoint(0), ofPoint(kinect.width, kinect.height)));
    gui.add(searchZonePmGroup);
    gui.setPosition(820, 10);
    gui.saveToFile("patchCloudParameters");
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();

    searchZone.setFromCenter(szCenter, sZwidth, sZheight);
    
    if(kinect.getHasNewFrame()){
            patchedImageCv = kinect.getPatchedCvImage(); // get the merged cvImage from the two kinects

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
                grayDiffOfImage.setFromPixels(grayDiff.getPixels(), kinect.width, kinect.height);
                
                // update the cv images
                grayDiffOfImage.flagImageChanged();

                // pass image on to contour finder
                contFinder.findContours(grayDiffOfImage.getCvImage());
            } else {
                contFinder.findContours(patchedImageCv.getCvImage());
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
    static map<int, bool> toSend;
    ofPixelsRef patchedImagePixRef = patchedImageCv.getPixelsRef(); // getPathcedImage
    ofColor tempClr;
    string colorName;
    bool bToSend;
    // new block = /newBlob <label> <color>
    for(int i=0; i< newLabels.size(); ++i){
        label = newLabels[i];
        area = ofxCv::toOf( contFinder.getBoundingRect(i) ); // get area of blob
        if(bSearchZoneOn){
            if (searchZone.inside(area)) {
                bToSend = true;
            } else {
                bToSend = false;
            }
            toSend.insert(pair<int, bool>(label, bToSend));

        } else {
            bToSend = true;
        }
        
        if(bToSend){
            ofxOscMessage m;
            m.setAddress("/newBlob");
            m.addIntArg( label );      // label
            tempClr = avgColor(area);                         // avg color of blob area
            cout << (colorName = clrNamer.nameColorGroup(tempClr) ) <<endl;
            if(colorName.length() == 0){
                colorName = lastFoundColorGroup;
            }
            m.addStringArg( colorName );     // send dat color name
            sender.sendMessage(m);
            lastFoundColorGroup = colorName;
        }
    }
    
    // block update = /currentBlob <label> <area> <age> <x> <y> <z>
    for(int i=0; i<contFinder.size(); ++i){
        label = contFinder.getLabel(i);
        if(bSearchZoneOn){
            bToSend = toSend.find( label )->second;
        } else {
            bToSend = true;
        }
        
        if(bToSend){
            ofxOscMessage m;
            m.setAddress("/currentBlob");
            m.addIntArg( label );        // label
            area = ofxCv::toOf( contFinder.getBoundingRect(i));
            m.addIntArg((area.width*area.height ));                // area
            m.addIntArg(tracker.getAge(label) );                   // age
            loc = ofxCv::toOf(contFinder.getCenter(i));
            m.addIntArg(loc.x);                                    // x
            m.addIntArg(loc.y);                                    // y
            m.addIntArg( patchedImagePixRef.getColor(loc.x, loc.y).getBrightness()); // z
            sender.sendMessage(m);
        }
    }
    
    // block dead = /deadBlob <label>
    for (int i=0; i<deadLabels.size(); ++i) {
        ofxOscMessage m;
        m.setAddress("/deadBlob");
        m.addIntArg((label = deadLabels[i] ));        // label
        toSend.erase( label ); // remove label from toSend map
        sender.sendMessage(m);
    }
#ifdef SHOW_SENDER_DEBUG
    cout << "new labels:     " << newLabels.size() << endl;
    cout << "current labels: " << contFinder.size() << endl;
    cout << "dead labels:    " << deadLabels.size() << endl<<endl;
#endif
}

//--------------------------------------------------------------
ofColor ofApp::avgColor(ofRectangle area){

    ofColor avgColor, currColor;
    ofPixelsRef greyDiffPix = grayDiffOfImage.getPixelsRef();
    unsigned int r, g, b;
    r = g = b = 0;
    int numberOfColoredPixels = 0;

    int stepSize = 5;
    int xloc, yloc;
    
    for(int x=0; x<area.width; x+=stepSize){
        for(int y=0; y<area.height; y+=stepSize){
            xloc = area.getTopLeft().x + x;
            yloc = area.getTopLeft().y + y;
            if(greyDiffPix.getColor(xloc, yloc).getBrightness() != 0){
                currColor = kinect.getPatchedColorAt(xloc, yloc);
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
//        avgColor = ofColor(r, g, b);
//        avgColor.blue;
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
            patchedImageCv.draw(0, 0);
        }
        contFinder.draw();
        
        if(bSearchZoneOn){
            ofSetColor(ofColor().red);
            ofNoFill();
            ofRect(searchZone);
            ofSetColor(255);
        }
        
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
	}
}