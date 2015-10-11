#include "redblock.h"
#include <math.h>


redblock::redblock()
{
	
}

void redblock::setup( int drawwidth, int drawheight)
{
    img.allocate(drawwidth,drawheight,OF_IMAGE_COLOR);

    drawWidth = drawwidth;
    drawHeight = drawheight;
}


redblock::~redblock(void)
{
}


void redblock::update(float x, float y, int brightness, float speed)
{

    ofSetColor(255, 255, 255);
    //ofRect(x, y, drawwidth, drawheight);

    float a,b,c;
    float noise;
    float color;
    ofColor kleurtje;
    
    for (int tempy=0; tempy<drawHeight; ++tempy) {
        for (int tempx=0; tempx<drawWidth; ++tempx) {

            a = tempx * .01;
            b = tempy * .01;
            c = ofGetFrameNum() / speed;
        
            noise = ofNoise(a,b,c) * 255;
            color = noise>brightness ? ofMap(noise,brightness,255,-20,255) : 0;

            kleurtje = noise > brightness ? ofColor(220, color, 0): 20;

            //float color = noise;

            img.setColor(tempx, tempy, kleurtje);
            //img.getPixels()[tempy*drawwidth+tempx] = color;
        }
    }

    img.update();
    img.reloadTexture();
}

ofImage redblock::getimage()
{
	return img;
}