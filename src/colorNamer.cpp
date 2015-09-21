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
    for(int i=0; i<colorList.numRows; ++i){
        temp.raiId = colorList.getInt(i, 0);
        rgb = colorList.getFromString( colorList.getString(i, 1), "-" );
        for(int j=0; j<3; ++j) temp.rgb[j] = ofToInt(rgb[j]);
        temp.hexCode = colorList.getString(i, 2);
        for(int j=0; j<NR_OF_LANGUAGES; ++j) temp.colorName[j] = colorList.getString(i, 3+j);
        
        row.push_back(temp);
        rgb.clear();
    }
    numRows = row.size(); 
}

string colorNamer::nameColor(ofColor nameMe, lang language){
}

string colorNamer::nameColorGroup(ofColor nameMe){}

string colorNamer::raiIDtoColorGroup(int ID){
    int id = floor(ID/1000);
    string colorGroup;
    switch (id) {
        case 1: colorGroup = "Beige and yellow";
            break;
        case 2: colorGroup = "orange";
            break;
        case 3: colorGroup = "red";
            break;
        case 4: colorGroup = "violet";
            break;
        case 5: colorGroup = "blue";
            break;
        case 6: colorGroup = "green";
            break;
        case 7: colorGroup = "gray";
            break;
        case 8: colorGroup = "brown";
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
                    //                    if(abs(b-g) < 65 && r>200){
                    //                        colorName = "pink";
                    //                        if(b < 65){
                    //                            colorName = "orange";
                    //                        }
                    //                    }
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