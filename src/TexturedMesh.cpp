//
//  TexturedMesh.cpp
//  Fortress_mapping
//
//  Created by Thijs Muijs on 10/3/15.
//
//

#include "TexturedMesh.h"


void TexturedMesh::setup(){
    
    int alpha = 255;
    blobColors[0] = ofColor(255, 0, 0, alpha);
    blobColors[1] = ofColor(0, 255, 0, alpha);
    blobColors[2] = ofColor(0, 0, 255, alpha);
    blobColors[3] = ofColor(255, 255, 0, alpha);
    blobColors[4] = ofColor(255, 0, 255, alpha);
    blobColors[5] = ofColor(0, 255, 255, alpha);
    blobColors[6] = ofColor(255, 127, 0, alpha);
    blobColors[7] = ofColor(127, 0, 255, alpha);
    blobColors[8] = ofColor(0, 255, 127, alpha);
    blobColors[9] = ofColor(127, 255, 0, alpha);
    blobColors[10]= ofColor(255, 0, 127, alpha);
    blobColors[11]= ofColor(0, 127, 255, alpha);
}

void TexturedMesh::update(){
    
    mesh.clear();
}

void TexturedMesh::draw(int x, int y, int width, int height, ofTexture &texture,ofPolyline &polyline, int label){


    ofPath pathFromContour;
    
    //CREATE PATHS FROM POLYLINES
    for(int j = 0; j < polyline.getVertices().size(); j++) {
        if(j == 0) {
            pathFromContour.newSubPath();
            pathFromContour.moveTo(polyline.getVertices()[j]);
        } else {
            pathFromContour.lineTo(polyline.getVertices()[j]);
        }
    }
    
    
    /*
    XSmooth =( 0.5 * x )+ (0.5 * prevX);
    YSmooth = (0.5 * y) + (0.5 * prevY);
    prevX = x;
    prevY = y;
    
    widthSmooth = (0.5 * width) + (0.5 * prevWidth);
    heightSmooth = (0.5 * height) + (0.5 * prevHeight);
    prevWidth = width;
    prevHeight = height;
    */
    
    pathFromContour.close();
    pathFromContour.simplify();
    
    float scaleX = (texture.getWidth()/width);
    float scaleY = (texture.getHeight()/height);
    
    mesh = pathFromContour.getTessellation();
    for(auto & v: mesh.getVertices()){
        mesh.addTexCoord(texture.getCoordFromPoint((v.x- x)*scaleX, (v.y- y)*scaleY ));
    }
    
    //ofSetColor(blobColors[label % 12]);
    ofSetColor(255);
    texture.bind();
    mesh.draw();
    texture.unbind();
    
}