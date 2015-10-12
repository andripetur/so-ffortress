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
    angle = 0;
    hasNewFrame = false; 
    for(int i=0; i<N_OF_KINECTS; ++i){
        kinect[i].setRegistration(true);
        kinect[i].init(false, true);
        kinect[i].open();
        kinect[i].setCameraTiltAngle(angle);
        
        grayImage[i].allocate(kinect[0].width, kinect[0].height);
        colorImage[i].allocate(kinect[0].width, kinect[0].height);
    }
    width = kinect[0].width;
    height = kinect[0].height;
    patchedImageCv.allocate(kinect[0].width, kinect[0].height);
    patchedImageColor.allocate(kinect[0].width, kinect[0].height);
    
    frameCount = 0;
    bNewAccumedFrame = false;
    for(int i=0; i<N_FRAMES+1; ++i){
        frames[i].allocate(width, height);
        clrFrames[i].allocate(width, height);
    }
    
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
    patchCloudParameters.add(bUseFrameSmoothing.set("FrameSmoothing", true));
    patchCloudParameters.add(bUseClrAdjustment.set("ColorAdjustment", true));

    viewPointCloudParameters.setName("viewpPointCloudParams");
    viewPointCloudParameters.add(pointCloudFarClipping.set("P.C. Far Clip", 0, 0, 10000));
    viewPointCloudParameters.add(stepSize.set("stepSize", 4, 1, 10));
    viewPointCloudParameters.add(pointSize.set("pointSize", 4, 1, 10));
    viewPointCloudParameters.add(bEnableMouseInput.set("Mouse Input", true));
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
    for(int i=0; i<N_OF_KINECTS;++i) kinect[i].update();
    
    // there is a new frame and we are connected
    if(kinect[0].isFrameNew()) {
        
        for(int i=0;i<N_OF_KINECTS;++i){
            // load grayscale depth image from the kinect source
            grayImage[i].setFromPixels(kinect[i].getDepthPixels(), kinect[i].width, kinect[i].height);
            colorImage[i].setFromPixels(kinect[i].getPixels(), kinect[i].width, kinect[i].height);
        }
        
        static float ** adj;
        adj = adjustPointClouds(&kinect[0], &kinect[1]);
        
        mergeGrayImages(grayImage[0], grayImage[1], adj);
        mergeColorImages(colorImage[0], colorImage[1], adj);
        
        if(bUseFrameSmoothing){
            frames[frameCount] = patchedImageCv;
            clrFrames[frameCount] = patchedImageColor;
            
            frameCount++;
            frameCount %= N_FRAMES;
            if(frameCount == 0) bNewAccumedFrame = true;
            
            if(bNewAccumedFrame){
                frameSmoother();
                patchedImageCv = frames[N_FRAMES];
                patchedImageColor = clrFrames[N_FRAMES];
                bNewAccumedFrame = false;
                hasNewFrame = true;
            }
        } else {
            if(bUseClrAdjustment) clrAdjust(&patchedImageColor);
            hasNewFrame = true;
        }
    }
}

void pointCloudStitcher::frameSmoother(){
    static bool bFirstTime = true;
    static int ** pixels;
    static int *** clrPixels;
    static ofPixels pixs;
    static ofPixels clrPixs;
    // make array
    if(bFirstTime){
        pixels = new int *[width];
        clrPixels = new int **[width];
        pixs.allocate(width, height, 1);
        clrPixs.allocate(width, height, 3); 
        
        for(int i = 0; i<width; ++i){
            pixels[i] = new int [height];
            clrPixels[i] = new int *[height];
        }
    
        for(int x=0; x<width; ++x){
            for(int y=0; y<height; ++y){
                clrPixels[x][y] = new int[3];
            }
        }
        
        bFirstTime = false;
    }
    
    // reset arrays to zero
    for(int y = 0; y < height; ++y ) {
        for(int x = 0; x < width; ++x) {
            pixels[x][y] = 0;
            for(int c=0; c<3; ++c){
                clrPixels[x][y][c] = 0;
            }
        }
    }
    
    // add and then divide by n
    for(int n=0; n<N_FRAMES; ++n) {
        ofPixelsRef frame = frames[n].getPixelsRef();
        ofPixelsRef clrFrame = clrFrames[n].getPixelsRef();
        
        for(int y = 0; y < height; ++y ) {
            for(int x = 0; x < width; ++x) {
                pixels[x][y] += frame.getColor(x, y)[0];
                for(int c=0; c<3; ++c){
                    clrPixels[x][y][c] += clrFrame.getColor(x, y)[c];
                }
                if(n == N_FRAMES-1){
                    pixs.setColor(x, y, ofColor( pixels[x][y]/N_FRAMES));
                    clrPixs.setColor(x, y, ofColor(
                                                   clrPixels[x][y][0]/N_FRAMES,
                                                   clrPixels[x][y][1]/N_FRAMES,
                                                   clrPixels[x][y][2]/N_FRAMES  ) );
                }
            }
        }
    }
    
    // store output in last array index.
    frames[N_FRAMES].setFromPixels(pixs);
    clrFrames[N_FRAMES].setFromPixels(clrPixs);
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
        
        patchedImageColor.draw(10, 310, 400, 300); // merged ColorImage
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

void pointCloudStitcher::mergeGrayImages(ofxCvGrayscaleImage imgOne, ofxCvGrayscaleImage imgTwo, float **adj){
    ofPixelsRef imgOnePix = imgOne.getPixelsRef();
    ofPixelsRef imgTwoPix = imgTwo.getPixelsRef();
    
    patchedImageCv.setFromPixels(merge(imgOnePix, imgTwoPix, adj, imgOne.width, imgTwo.height));
}

void pointCloudStitcher::mergeColorImages(ofxCvColorImage imgOne, ofxCvColorImage imgTwo, float **adj){
    ofPixelsRef imgOnePix = imgOne.getPixelsRef();
    ofPixelsRef imgTwoPix = imgTwo.getPixelsRef();
    
    patchedImageColor.setFromPixels(merge(imgOnePix, imgTwoPix, adj, imgOne.width, imgTwo.height));
}

ofPixelsRef pointCloudStitcher::merge(ofPixelsRef imgOnePix, ofPixelsRef imgTwoPix, float **adj, int width, int height){
    ofPoint tempPointCurrent;
    bool bIsColored = (imgOnePix.getNumChannels() > 1);
    
    int offsetX, offsetY;
    int brightnessOne, brightness;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            brightnessOne = imgTwoPix.getColor(x, y).getBrightness();
            if(brightnessOne>0) {
                offsetX = int(x * adj[x][y]);
                offsetY = y+kinectDistanceY;
                if(offsetX < width && offsetX > 0) {
                    if(offsetY < height && offsetY > 0){
//                        if(bIsColored){
//                            imgOnePix.setColor(offsetX, offsetY, imgTwoPix.getColor(x, y));
//                        } else {
                            if( imgOnePix.getColor(offsetX, offsetY).r > brightnessOne ){
                                imgOnePix.setColor(offsetX, offsetY, imgTwoPix.getColor(x, y));
                            }
//                        }
                    }
                }
                
            }
        }
    }
    
    return imgOnePix;
}

ofPixels pointCloudStitcher::clrAdjust(ofxCvColorImage *img){
    static bool bFirstTime = true;
    static ofPixels saturated, image;
    if(bFirstTime){
        saturated.allocate(width, height, 3);
        image.allocate(width, height, 3);
        bFirstTime = false;
    }
    image.setFromPixels(img->getPixels(), image.getWidth(), image.getHeight(), 3);
    
    ofColor currColor;
    float brighter = (255 - sampleBrightness(image, 10)) * 0.75;
    cout<<"brightness: " << brighter << endl;
    for(int x=0; x<image.getWidth(); ++x){
        for(int y=0; y<image.getHeight(); ++y){
            currColor = image.getColor(x, y);
            currColor.setBrightness( currColor.getBrightness() + brighter );
            currColor.setSaturation( currColor.getSaturation()* + brighter);
            saturated.setColor(x, y, currColor);
        }
    }
    
    return saturated;
}

float pointCloudStitcher::sampleBrightness(ofPixels imgPixs, int steps){
    float brightness = 0;
    int howManySamples = 0;
    for(int x=0; x<imgPixs.getWidth(); x+=steps){
        for(int y=0; y<imgPixs.getHeight(); y+=steps){
            brightness += imgPixs.getColor(x, y).getBrightness();
            ++howManySamples;
        }
    }
    
    return brightness/(float)howManySamples;
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
    for(int i=0; i<N_OF_KINECTS; ++i){
        kinect[i].setCameraTiltAngle(0); // zero the tilt on exit
        kinect[i].close();
    }
}

void pointCloudStitcher::keyListener(ofKeyEventArgs & a){
    switch (a.key) {
        case ' ':
            bEnableMouseInput = !bEnableMouseInput;
            break;
            
        case'p':
            bDrawPointCloud = !bDrawPointCloud;
            break;
            
        case'a':
            bUseClrAdjustment = !bUseClrAdjustment;
            break;
            
        case 'o':
            for(int i=0;i<N_OF_KINECTS;++i) {
                kinect[i].setCameraTiltAngle(angle); // go back to prev tilt
                kinect[i].open();
            }
            break;
            
        case 'c':
            for(int i=0;i<N_OF_KINECTS;++i) {
                kinect[i].setCameraTiltAngle(0); // zero the tilt
                kinect[i].close();
            }
            break;
            
        case'u':
        case'U':
            bUseFrameSmoothing = !bUseFrameSmoothing;
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            for(int i=0;i<N_OF_KINECTS;++i) kinect[i].setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            for(int i=0;i<N_OF_KINECTS;++i) kinect[i].setCameraTiltAngle(angle);
            break;
    }
}

bool pointCloudStitcher::isConnected(){
    return kinect[0].isConnected();
}
string pointCloudStitcher::isItConnectedString(){
    if(kinect[0].isConnected()){
        return "yes";
    } else {
        return "no";
    }
}

bool pointCloudStitcher::isPointCloudDrawn(){
    return bDrawPointCloud;
}
bool pointCloudStitcher::getHasNewFrame(){
    if(hasNewFrame){
        hasNewFrame=false;
        return true;
    } else {
        return false; 
    }
}
ofxCvGrayscaleImage pointCloudStitcher::getPatchedCvImage(){
    return patchedImageCv;
}
ofxCvColorImage pointCloudStitcher::getPatcheColorImage(){
    return patchedImageColor;
}
ofColor pointCloudStitcher::getPatchedColorAt(int x, int y){
    return patchedImageColor.getPixelsRef().getColor(x, y);
}
ofColor pointCloudStitcher::getColorAt(int x, int y){
    return kinect[0].getColorAt(x, y);
}
ofVec2f pointCloudStitcher::getWH2fvec(){
    return ofVec2f(width, height); 
}