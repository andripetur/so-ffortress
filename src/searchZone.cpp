//
//  searchZone.cpp
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 11/10/15.
//
//

#include "searchZone.h"
void searchZone::setup(ofVec2f kWH){
    k = kWH;
    setupParameters();
}

void searchZone::setupParameters(){
    // search zone pm's
    parameters.setName("SearchZonePms");
    parameters.add(bSearchZoneOn.set("SearchZoneEnable", false));
    parameters.add(sZwidth.set("width", 100, 2, k.x));
    parameters.add(sZheight.set("height", 100, 2, k.y));
    ofPoint kinBoRi(k.x, k.y);
    parameters.add(szCenter.set("center", kinBoRi*0.5, ofPoint(0), kinBoRi ));
}

void searchZone::update(){
    searchZone.setFromCenter(ofPoint(szCenter), sZwidth, sZheight);
}

void searchZone::draw(){
    if(bSearchZoneOn){
        ofSetColor(ofColor().red);
        ofNoFill();
        ofRect(searchZone);
    }
}

bool searchZone::isOn(){
    return bSearchZoneOn;
}
bool searchZone::isInside(ofRectangle area){
    return searchZone.inside(area);
}