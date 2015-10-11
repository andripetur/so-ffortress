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
    void drawLabelColorMap(); 
    void setupParameters();
    ofParameterGroup            videoParameters;
    ofParameterGroup            redBlockParameters;
    
    enum BLOCK_TYPE{ RED_BLOCK, BLUE_BLOCK, YELLOW_BLOCK, WHITE_BLOCK, BLACK_BLOCK };
    // map label, blockType
    map<int, int> blockMap;
    
    int clrStngToBlkT(string clr);
    string BlkTyToStngColor(int blT);
    
private:
    ofParameter<float>          scale, moveX, moveY;
    ofParameter<float>          brightness, speed;
    ofParameter<bool>           bDrawContours;
    
    int                         W,H;
    redblock                    redBlock;
    blueblock                   blueBlock;
    greenblock                  greenBlock;
    yellowblock                 yellowBlock;
    TexturedMesh                texturedMesh;
};



#endif /* defined(__Fortress_mapping__videoProjection__) */
