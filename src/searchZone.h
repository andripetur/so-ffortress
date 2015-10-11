//
//  searchZone.h
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 11/10/15.
//
//
#include "ofMain.h"

#ifndef __ffortress__searchZone__
#define __ffortress__searchZone__

class searchZone{
public:
    void setup(ofVec2f kWH);
    void setupParameters();
    void update();
    void draw();
    
    ofParameterGroup parameters;
    bool isOn();
    bool isInside(ofRectangle area); 
private:
    ofVec2f k; 
    // search zone
    ofRectangle searchZone;
    ofParameter<bool> bSearchZoneOn;
    ofParameter<float> sZwidth, sZheight;
    ofParameter<ofVec2f> szCenter;
};
#endif /* defined(__ffortress__searchZone__) */
