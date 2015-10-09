//
//  colorNamer.cpp
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 21/09/15.
//
//

#include "colorNamer.h"
colorNamer::colorNamer(){
    numCols = 9;
}

void colorNamer::setup(){
    colorList.loadFile("/Applications/of_v0.8.4_osx_release/apps/ffortress/ffortress/bin/data/ral_colors.csv", ",", "#" );
    vector<string> rgb;
    collumn temp;
    ofColor lab;

    for(int i=0; i<colorList.numRows; ++i){
        //get rai xxx id, and strip "rai "
        temp.raiId = ofToInt(colorList.getString(i, 0).erase(0, 4));
        rgb = colorList.getFromString( colorList.getString(i, 1), "-" );
        for(int j=0; j<3; ++j) temp.rgb[j] = ofToInt(rgb[j]);
        temp.hexCode = colorList.getString(i, 2);
        for(int j=0; j<NR_OF_LANGUAGES; ++j) temp.colorName[j] = colorList.getString(i, 3+j);
        lab = ofxCv::convertColor(ofColor( temp.rgb[0], temp.rgb[1], temp.rgb[2] ), CV_RGB2Lab);
        for(int j=0; j<3; ++j) temp.lab[j] = lab[j];
        
        row.push_back(temp);
        rgb.clear();
    }
    numRows = row.size(); 
}

string colorNamer::nameColor(ofColor nameMe, lang language){
    string colorName;
    int rowNr = findRowOfNearestColor(nameMe);
    colorName = row[rowNr].colorName[language];
    
    return colorName;
}

string colorNamer::nameColorGroup(ofColor nameMe){
    string colorGroup;
    int rowNr = findRowOfNearestColor(nameMe);
    colorGroup = raiIDtoColorGroup( row[rowNr].raiId );
    return colorGroup; 
}

string colorNamer::getGroupOfLastFoundColor(){
    string colorGroup;
    colorGroup = raiIDtoColorGroup( row[lastFoundColorRow].raiId );
    return colorGroup;
}

int colorNamer::findRowOfNearestColor(ofColor nameMe){
    float smallestDistance = MAXFLOAT;
    int smallestDistanceRow = 0;
    float currDistance = 0;
    ofColor lab;
    
    // distance in Lab colorspace
    lab = ofxCv::convertColor(nameMe, CV_RGB2Lab);
    for(int i=0;i<numRows;++i){
        currDistance = pow((float)(lab.r-row[i].lab[0]),2)
        + pow((float)(lab.g-row[i].lab[1]),2)
        + pow((float)(lab.b-row[i].lab[2]),2);
        if(currDistance < smallestDistance){
            smallestDistance = currDistance;
            smallestDistanceRow = i;
        }
    }
    
    lastFoundColorRow = smallestDistanceRow;
    return smallestDistanceRow;
}

string colorNamer::raiIDtoColorGroup(int ID){
    int id = floor(ID/1000);
    string colorGroup;
    switch (id) {
        case 1: colorGroup = "yellow"; //beige and yellow
            break;
        case 2:
//            colorGroup = "orange";
//            break;
        case 3: colorGroup = "red";
            break;
        case 4:
//            colorGroup = "violet";
//
            break;
        case 7:
        case 8:
        case 5: colorGroup = "blue";
            break;
        case 6: colorGroup = "green";
            break;
//        case 7: colorGroup = "gray";
//            break;
//        case 8: colorGroup = "brown";
            break;
        case 9:
            if(ID == 9004 ||ID == 9005 ||ID == 9011 ||ID == 9017){
                colorGroup = "black";
            } else {
                colorGroup = "white";
            }
            break;
    }
    return colorGroup;
}

ofColor colorNamer::getColorByName(string clr, lang language){
    ofColor color;
    for(int i=0;i<numRows;++i){
        if(clr == row[i].colorName[language]){
            color = ofColor(row[i].rgb[0],row[i].rgb[1], row[i].rgb[2] );
            i = numRows; 
        }
    }
    return color;
}

string colorNamer::nameColorConditional(ofColor nameMe){
    int r = nameMe.r;
    int g = nameMe.g;
    int b = nameMe.b;
    string colorName;
    int grens = 39;
    
    if(nameMe == ofColor(0)){ // check if black
        colorName = "black";
    } else if( nameMe == ofColor(255) ){ // check if white
        colorName = "white";
    } else {
        if(abs(r - g) < 10 && abs(r - b) < 10){       // if r,g,b are same = grey
            colorName = "gray";
        } else {
            if(r > b && r>g) {
                if(abs(r-g) < grens){
                    colorName = "yellow";
                } else if( r > g ){
                    colorName = "red";
                } else {
                    if(g > b){
                        colorName = "green";
                    } else {
                        colorName = "blue";
                    }
                }
                if(abs(b - r) < grens*2 && abs(b-g) > grens){
                    colorName = "Violet";
                }
            } else if( g > r ){
                if(abs(g - b) < grens) {
                    //                    if( g < 56 ){
                    if( abs(g-b) < 10){
                        colorName = "blue";
                    } else if(b > g){
                        colorName = "blue";
                    } else {
                        if(abs(r - g) < grens){
                            colorName = "yellow";
                        } else {
                            colorName = "green";
                        }
                    }
                } else if(g > b) {
                    colorName = "green";
                } else {
                    colorName = "blue";
                }
            } else if( b>g ){
                if(abs(b - r) < grens+30){
                    colorName = "Violet";
                } else if( b>r ){
                    colorName = "blue";
                }
            }
            
        }
        
    }
    
    return colorName;
}