#pragma once
#include "ofMain.h"
#include "mypoint.h"

class blueblock
{
public:
	blueblock();
	void setup(int drawwidth, int drawheight);
	~blueblock(void);
	void update(float x, float y, int randomness, int length, float speed, int counter);
	ofImage getimage();

private:
	int nextflash;
	int nextflashlength;
	ofImage img;
     int drawWidth, drawHeight;

};

