#pragma once
#include "ofMain.h"
#include "treefractal.h"

class greenblock
{
public:
	greenblock();
	~greenblock();

	void update(float counter);
	void setup(float drawwidth, float drawheight);

	ofFbo getfbo();


private:
	float x;
	float y;
	float drawwidth;
	float drawheight;
	ofFbo fbo;
	treefractal tree;
};

