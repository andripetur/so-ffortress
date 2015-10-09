#pragma once
#include "ofMain.h"
#include "mypoint.h"

class yellowblock
{
public:
	yellowblock();
	void setup(int drawwidth, int drawheight);
	~yellowblock(void);
	void update(float speed, int counter);
	ofFbo getfbo();

private:
	float next;
	ofFbo fbo;
	int drawwidth;
	int drawheight;

};

