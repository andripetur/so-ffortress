//
//  pointCloudStitcher.h
//  kinectExample
//
//  Created by Student on 12/09/15.
//
//

#ifndef __kinectExample__pointCloudStitcher__
#define __kinectExample__pointCloudStitcher__
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"

class PointCloudStitcher{
public:
    PointCloudStitcher();
    
    void setup();
    void update();
    void draw();
    
    void backgroundExtraction();
    
    
};

#endif /* defined(__kinectExample__pointCloudStitcher__) */
