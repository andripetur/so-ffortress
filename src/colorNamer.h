//
//  colorNamer.h
//  ffortress
//
//  Created by Andri Pétur Þrastarson on 21/09/15.
//
//
#include "ofxCsv.h"
#include "ofxCv.h"
#include "ofApp.h"
#include "string.h"

#ifndef __ffortress__colorNamer__
#define __ffortress__colorNamer__

enum lang{
    GERMAN = 0,
    ENGLISH,
    FRENCH,
    SPANISH,
    ITALIAN,
    DUTCH,
    NR_OF_LANGUAGES
};

class colorNamer{
public:
    colorNamer();
    void setup();
    string nameColorConditional(ofColor nameMe); 
    string nameColor(ofColor nameMe, lang language);
    string nameColorGroup(ofColor nameMe);
    string raiIDtoColorGroup(int ID);
    
    string getGroupOfLastFoundColor();
    
    ofColor getColorByName(string clr, lang language); 
    
    struct collumn{
        int raiId;
        string hexCode;
        int rgb[3];
        int lab[3];
        string colorName[NR_OF_LANGUAGES];        
    };
    
    vector<collumn> row; 
    
    int numCols, numRows;
    int lastFoundColorRow;
private:
    // finds closest color, returns row
    int findRowOfNearestColor(ofColor nameMe);
    wng::ofxCsv colorList;
};
#endif /* defined(__ffortress__colorNamer__) */
