//
//  videoProjection.h
//  Fortress_mapping
//
//  Created by Thijs Muijs on 10/9/15.
//
//

#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"

//class for texturing and creating a mesh from contours
#include "TexturedMesh.h"
//visuals
#include "redblock.h"
#include "blueblock.h"
#include "greenblock.h"
#include "yellowblock.h"

#ifndef __Fortress_mapping__videoProjection__
#define __Fortress_mapping__videoProjection__

class videoProjection{
public:

    void setup();
    void update();
    void draw(ofxCv::ContourFinder &contourFinder);
    void setupParameters();
    ofParameterGroup            videoParameters;
    ofParameterGroup            redBlockParameters;

private:
    ofParameter<float>          scale, moveX, moveY;
    ofParameter<float>          brightness, speed;
    
    int                         W,H;
    redblock                    redBlock;
    blueblock                   blueBlock;
    greenblock                  greenBlock;
    yellowblock                 yellowBlock;
    TexturedMesh                texturedMesh;
};



#endif /* defined(__Fortress_mapping__videoProjection__) */
