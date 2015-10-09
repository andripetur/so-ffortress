#pragma once
#include "ofMain.h"
//#include "mypoint.h"

class redblock
{
public:
	redblock();
	void setup(int drawwidth, int drawheight);
	~redblock(void);
	void update(float x, float y, int brightness, float speed);
	ofImage getimage();

private:
	ofImage img;
    int drawWidth, drawHeight;

};

