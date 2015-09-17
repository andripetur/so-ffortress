#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    // zero the tilt on startup
    angle = 10;
    
    for(int i=0; i<amt; ++i){
        kinect[i].setRegistration(true);
        kinect[i].init(false, true);
        kinect[i].open();
        kinect[i].setCameraTiltAngle(angle);
        
        grayImage[i].allocate(kinect[0].width, kinect[0].height);
        grayDiff[i].allocate(kinect[0].width, kinect[0].height);
        grayDiffOfImage[i].allocate(kinect[0].width, kinect[0].height);
        bgImage[i].allocate(kinect[0].width, kinect[0].height);
        bLearnBackground[i] = false;
        bBackgroundLearned[i] = false;
        bForgetBackground[i] = false;
    }
	
    patchedImage.allocate(kinect[0].width, kinect[0].height);
    patchedImagePixels.allocate(kinect[0].width, kinect[0].height, OF_PIXELS_MONO);
    patchedImageCv.allocate(kinect[0].width, kinect[0].height);
    
	ofSetFrameRate(60);
		
	// start from the front
	bDrawPointCloud = false;
        
    // open an outgoing OSC connection to HOST:PORT
    sender.setup(HOST, PORT);
        
    setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
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
    patchCloudParameters.add(bShowInfo.set("info", false));
    gui.add(patchCloudParameters);
    gui.setPosition(820, 10);
    gui.saveToFile("patchCloudParameters");
}

//--------------------------------------------------------------
void ofApp::renderPatchCloud(){
    patchedImage.begin();
    if(bWorldToCamera)easyCam.begin();
        ofClear(0);
        glEnable(GL_POINT_SMOOTH);
        glDepthMask(GL_FALSE);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        mesh.setMode(OF_PRIMITIVE_POINTS);
    ofPushMatrix();
            ofScale(1, 1, -1);
            ofTranslate(0, 0, -1000); // center the points a bit
            glPointSize(pointSize);
            mesh.draw();
    ofPopMatrix();
        ofDisableBlendMode();
        glDisable(GL_POINT_SMOOTH);
    if(bWorldToCamera)easyCam.end();
    patchedImage.end();
}

//--------------------------------------------------------------
void ofApp::update() {
	
	ofBackground(100, 100, 100);
    if(bDrawPointCloud)easyCam.setFarClip(pointCloudFarClipping);
	
    for(int i=0; i<amt;++i) kinect[i].update();

	// there is a new frame and we are connected
    if(kinect[0].isFrameNew()) {
		
        for(int i=0;i<amt;++i){
            // load grayscale depth image from the kinect source
            grayImage[i].setFromPixels(kinect[i].getDepthPixels(), kinect[i].width, kinect[i].height);
    
//            // set new background image
//            if(bLearnBackground[i]){
//                bgImage[i] = grayImage[i];   // let this frame be the background image from now on
//                bLearnBackground[i] = false;
//                bBackgroundLearned[i] = true;
//            }
//            
//            if(bForgetBackground[i]){
//                bBackgroundLearned[i] = false;
//                bForgetBackground[i] = false;
//            }
//            
//            if(bBackgroundLearned[i]){
//                cvAbsDiff(bgImage[i].getCvImage(), grayImage[i].getCvImage(), grayDiff[i].getCvImage());
//                cvErode(grayDiff[i].getCvImage(), grayDiff[i].getCvImage(), NULL, 2);
//                cvDilate(grayDiff[i].getCvImage(), grayDiff[i].getCvImage(), NULL, 1);
//                // threshold ignoring little differences
//                cvThreshold(grayDiff[i].getCvImage(), grayDiff[i].getCvImage(), kinectFarClipping, 255, CV_THRESH_BINARY);
//                grayDiff[i].flagImageChanged();
//                // update the ofImage to be used as background mask for the blob finder
//                grayDiffOfImage[i].setFromPixels(grayDiff[i].getPixels(), kinect[0].width, kinect[0].height);
//                
//                // update the cv images
//                grayDiffOfImage[i].flagImageChanged();
            }
            
        }
        float ** adj;
        adj = mergePointclouds(&kinect[0], &kinect[1]);
        
//        if(bBackgroundLearned[0]){
//            mergeGrayImages(grayDiffOfImage[0], grayDiffOfImage[1], adj);
//        } else {
            mergeGrayImages(grayImage[0], grayImage[1], adj);
//        }
    
    // set new background image
    if(bLearnBackground[0]){
        bgImage[0] = patchedImageCv;   // let this frame be the background image from now on
        bLearnBackground[0] = false;
        bBackgroundLearned[0] = true;
    }
    
    if(bForgetBackground[0]){
        bBackgroundLearned[0] = false;
        bForgetBackground[0] = false;
    }
    
    if(bBackgroundLearned[0]){
        grayImage[0] = patchedImageCv;
        cvAbsDiff(bgImage[0].getCvImage(), grayImage[0].getCvImage(), grayDiff[0].getCvImage());
        cvErode(grayDiff[0].getCvImage(), grayDiff[0].getCvImage(), NULL, 3);
        cvDilate(grayDiff[0].getCvImage(), grayDiff[0].getCvImage(), NULL, 1);
        // threshold ignoring little differences
        cvThreshold(grayDiff[0].getCvImage(), grayDiff[0].getCvImage(), 20, 255, CV_THRESH_BINARY);
        grayDiff[0].flagImageChanged();
        // update the ofImage to be used as background mask for the blob finder
        grayDiffOfImage[0].setFromPixels(grayDiff[0].getPixels(), kinect[0].width, kinect[0].height);
        
        // update the cv images
        grayDiffOfImage[0].flagImageChanged();
    
//        renderPatchCloud();
//        patchedImage.readToPixels(patchedImagePixels);
//        patchedImageCv.setFromPixels(patchedImagePixels);

        // find contours which are between the size of 100 pixels and 1/3 the w*h pixels.
        contourFinder.findContours(grayDiffOfImage[0], 100, (kinect[0].width*kinect[0].height)/2, 20, false);
	} //isFrameNew

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
                currColor = kinect[0].getColorAt(area.x + x, area.y + y);
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
float** ofApp::mergePointclouds(ofxKinect* kinectOnePtr, ofxKinect* kinectTwoPtr){
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

//--------------------------------------------------------------
void ofApp::mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float ** adj){
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
void ofApp::draw() {
	
	ofSetColor(255, 255, 255);
	
	if(bDrawPointCloud) {
        if(bEnableMouseInput){
            easyCam.enableMouseInput();
        } else {
            easyCam.disableMouseInput();
        }
        
		easyCam.begin();
            drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinect[0].drawDepth(10, 10, 200, 150);
        kinect[1].drawDepth(210, 10, 200, 150);
        
		kinect[0].draw(10, 160, 200, 150); //rgb
		kinect[1].draw(210, 160, 200, 150); //rgb
        
        grayDiffOfImage[0].draw(10, 320, 200, 150);
        grayDiffOfImage[1].draw(210, 320, 200, 150);
        
		contourFinder.draw(10, 320, 200, 150);
        
//        patchedImageCv.draw(420, 10, 400, 300);
        grayDiffOfImage[0].draw(420, 10, 400, 300);
        contourFinder.draw(420, 10, 400, 300);
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
        << " c to close connection, o to open it again, connection is: " << kinect[0].isConnected() << endl;
    
        ofDrawBitmapString(reportStream.str(), 20, ofGetWindowHeight()*0.75);
    }
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::drawPointCloud() {
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

//--------------------------------------------------------------
void ofApp::exit() {
    for(int i=0; i<amt; ++i){
        kinect[i].setCameraTiltAngle(0); // zero the tilt on exit
        kinect[i].close();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case ' ':
            bEnableMouseInput = !bEnableMouseInput;
			break;
            
        case 'b':
            for(int i=0;i<amt;++i) bLearnBackground[i] = true;
            break;
            
        case 'f':
            for(int i=0;i<amt;++i) bForgetBackground[i] = true;
            break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
            
        case'i':
        case'I':
            bShowInfo = !bShowInfo;
            break;
            
        case'w':
        case'W':
            bWorldToCamera = !bWorldToCamera;
            break;
			
		case '>':
		case '.':

			break;
			
		case '<':
		case ',':

			break;
			
		case '+':
		case '=':

			break;
			
		case '-':

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