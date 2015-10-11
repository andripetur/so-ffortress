#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    // setup both windows
    winManager.setup((ofxAppGLFWWindowMulti *)ofGetWindowPtr());
    
    ofSetWindowTitle("Beam/Draw");
    
    winManager.createWindow();
    winManager.setWindowTitle(1, "GUI");
    winManager.setWindowShape(1, 300, ofGetScreenHeight());
    winManager.setWindowPosition(1, 0, 0);
    
    kinect.setup();
    blobTrackr.setup(kinect.getWH2fvec());
    clrNamer.setup();
    vidProjection.setup();
    
	ofSetFrameRate(10);
    // open an outgoing OSC connection to HOST:PORT
    sender.setup(HOST, PORT);
    
    setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    // Patchcloud Pm
    gui.add(kinect.patchCloudParameters);
    gui.add(kinect.viewPointCloudParameters);
    
    // main pm
    mainAppPm.setName("mainApp");
    mainAppPm.add(bShowInfo.set("info", false));
    mainAppPm.add(bShowLabelColors.set("show Label&Colors", false));
    mainAppPm.add(bDrawProjectionMapping.set("draw projection mapping", false));
    mainAppPm.add(host.set("host", HOST));
    gui.add(mainAppPm);
    
    gui.add(blobTrackr.blobTrackerPms);
    // search zone pm's
    gui.add(blobTrackr.searchZone.parameters);
    
    // projectionMapping gui
    gui.add( vidProjection.redBlockParameters );
    gui.add( vidProjection.videoParameters );

    // set position and save path
    gui.setPosition(10, 10);
    gui.minimizeAll();
    gui.saveToFile("patchCloudParameters");
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();
    
    if( kinect.getHasNewFrame() ){
        blobTrackr.update(kinect.getPatchedCvImage());
    }
    
    vidProjection.update();
    oscSender();
}

//--------------------------------------------------------------
void ofApp::oscSender(){
//#define SHOW_SENDER_DEBUG
    ofxCv::ContourFinder* contourFinder = blobTrackr.getContourFinderPointer();
    ofxCv::RectTracker& tracker = contourFinder->getTracker();
    
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
    
    ofPoint loc;
    ofRectangle area;
    int label;
    static map<int, bool> toSend;  //map of labels and if they should be sent
    ofPixelsRef patchedImagePixRef = blobTrackr.getPatchedImageCvPixelRef(); // getPathcedImage
    ofColor tempClr;
    string colorName;
    bool bToSend; // temp bToSend
    
    // new block = /newBlob <label> <color>
    for(int i=0; i< newLabels.size(); ++i){
        label = newLabels[i];
        area = ofxCv::toOf( contourFinder->getBoundingRect(i) ); // get area of blob
        if( blobTrackr.searchZone.isOn() ){
            // send if inside search zone
            bToSend = blobTrackr.searchZone.isInside(area);
            toSend.insert(pair<int, bool>(label, bToSend));
        } else {
            bToSend = true;
        }
        
        if(bToSend){
            ofxOscMessage m;
            m.setAddress("/newBlob");
            m.addIntArg( label );      // label
            tempClr = avgColor(area);                         // avg color of blob area
//            cout << (colorName = clrNamer.nameColorGroup(tempClr) ) <<endl;
            if(colorName.length() == 0){
                colorName = lastFoundColorGroup;
            }
            m.addStringArg( colorName );     // send dat color name
            vidProjection.blockMap.insert(pair<int, int>(label, vidProjection.clrStngToBlkT( colorName ) ));
            sender.sendMessage(m);
            lastFoundColorGroup = colorName;
        }
    }
    
    // block update = /currentBlob <label> <area> <age> <x> <y> <z>
    for(int i=0; i<contourFinder->size(); ++i){
        label = contourFinder->getLabel(i);
        if( blobTrackr.searchZone.isOn() ){
            bToSend = toSend.find( label )->second;
        } else {
            bToSend = true;
        }
        
        if(bToSend){
            ofxOscMessage m;
            m.setAddress("/currentBlob");
            m.addIntArg( label );        // label
            area = ofxCv::toOf( contourFinder->getBoundingRect(i));
            m.addIntArg((area.width*area.height ));                // area
            m.addIntArg(tracker.getAge(label) );                   // age
            loc = ofxCv::toOf(contourFinder->getCenter(i));
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
        vidProjection.blockMap.erase( label ); // remove label from color map
        sender.sendMessage(m);
    }
#ifdef SHOW_SENDER_DEBUG
    cout << "new labels:     " << newLabels.size() << endl;
    cout << "current labels: " << contFinder.size() << endl << endl;
#endif
}

//--------------------------------------------------------------
ofColor ofApp::avgColor(ofRectangle area){

    ofColor avgColor, currColor;
    ofPixelsRef greyDiffPix = blobTrackr.getDrayDiffImagePixelRef();
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
//                currColor.setSaturation( currColor.getSaturation() * 2);
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
void ofApp::draw() {
    // check which windod we will be drawing to
    int activeWindow = winManager.getActiveWindowNo();
    
    ofSetColor(255, 255, 255);
    if( activeWindow == 0){ // if in main window draw to be beamed stuff:
        if(bDrawProjectionMapping){
            vidProjection.draw(blobTrackr.getContourFinderPointer()); // beam on blocks
        } else {
            ofBackground(100, 100, 100);

            kinect.draw(); // draw from kinect
            if(!kinect.isPointCloudDrawn()) {
                blobTrackr.draw(); // draw blobTrackr
            }
        }
    }else{ // if gui window draw gui elements
        ofBackground(60, 60, 60);
        ofSetColor(0);
        gui.draw();
        
        // draw info
        if(bShowInfo){
            ofSetColor(255, 255, 255);
            stringstream reportStream;
            
            reportStream
            << " b to learn background."<< endl
            << " f to forget background."<< endl
            << " space to dis/enable mouse input for pointcloud"<< endl
            << " num blobs found " << blobTrackr.getNumberOfActiveBlobs()
            << " fps: " << ofGetFrameRate() << endl
            << " c to close connection, o to open it again, connection is: " << kinect.isConnected() << endl;
            
            ofDrawBitmapString(reportStream.str(), 20, ofGetWindowHeight()*0.75);
        }
        
        if(bShowLabelColors){
            ofSetColor(255);
            vidProjection.drawLabelColorMap();
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
        case'i':
        case'I':
            bShowInfo = !bShowInfo;
            break;
            
        case'l':
            case'L':
            bShowLabelColors = !bShowLabelColors;
            break;
	}
}