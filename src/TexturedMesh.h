//
//  TexturedMesh.h
//  Fortress_mapping
//
//  Created by Thijs Muijs on 10/3/15.
//
//
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"


#ifndef __Fortress_mapping__TexturedMesh__
#define __Fortress_mapping__TexturedMesh__


class TexturedMesh
{
public:
    void setup();
    void update();
    void draw(int x, int y, int width, int height, ofTexture &texture, ofPolyline &polyline, int label);

    
private:
    ofMesh mesh;
    //ofTexture* texture;
    int alpha = 50;
    
    ofColor                     blobColors[12];
    /*
    int XSmooth;
    int prevX;
    int YSmooth;
    int prevY;
    
    int widthSmooth;
    int prevWidth;
    int heightSmooth;
    int prevHeight;
     */
};





#endif /* defined(__Fortress_mapping__TexturedMesh__) */
