#pragma once
#include "ofMain.h"
#include "mypoint.h"

class treefractal
{
public:
	treefractal();
	void setup(float startx, float starty);
	~treefractal(void);
	void draw(float branchlength, float angle, float numiterations, float lengthmultiplier, float startangle, int randomseed, float randomness);


private:
	float startx, starty;
};

