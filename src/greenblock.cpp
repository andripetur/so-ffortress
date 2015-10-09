#include "greenblock.h"


greenblock::greenblock()
{
}

greenblock::~greenblock()
{
}

void greenblock::setup(float ndrawwidth, float ndrawheight)
{

	x = 0;
	y = 0;

	drawwidth = ndrawwidth;
	drawheight= ndrawheight;
	fbo.allocate(drawwidth, drawheight);

	tree.setup(x + (0.5 * drawwidth), y + (0.5 * drawheight));
}

void greenblock::update(float counter)
{
	
	fbo.begin();
	ofSetColor(ofColor(0,0,0));
	ofRect(x, y, drawwidth, drawheight);



	ofSetColor(255, 255, 255);
	ofCircle(x + 0.5 * drawwidth, y + 0.5 * drawheight, 3);

	tree.draw(20.3, 20, counter / 500,0.9,0,3,1);
	tree.draw(20.2, 21, counter / 501,0.91,89, 2 ,0.5);
	tree.draw(21, 19, counter / 490,0.91,188, 1, 1);
	tree.draw(20.5, 20.5, counter / 502,0.89,273, 5, 0.7);
	fbo.end();
}

ofFbo greenblock::getfbo()
{
	return fbo;
}