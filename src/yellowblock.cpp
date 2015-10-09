#include "yellowblock.h"
#include <math.h>


yellowblock::yellowblock()
{
	
}

void yellowblock::setup(int ndrawwidth, int ndrawheight)
{
	drawwidth = ndrawwidth;
	drawheight = ndrawheight;

	next = 0;
	fbo.allocate(drawwidth,drawheight);

}


yellowblock::~yellowblock(void)
{
}


void yellowblock::update(float speed, int counter)
{
	if (next == 0){
		next = counter;
	}
	
	fbo.begin();

	ofSetColor(ofColor(255,255,255));
	ofRect(0, 0, drawwidth, drawheight);

	float middlex = 0.5 * drawwidth;
	float middley = 0.5 * drawheight;

	float tussenstap = (middlex * middlex) + (middley * middley);

	float maxradius = sqrt(tussenstap);

    float r;
	for (int i = maxradius; i > 0; i-=15)
	{
		if(counter > next + i && counter < next + i + 20)
		{
			r = i + 10;

		}
		else
		{
			r = i;
		}


		float blue = ofMap(i, 0, maxradius, 0, 255);
		float green = ofMap(i, 0, maxradius, 230, 255);

		ofSetColor(255, green, blue);
		ofCircle(middlex, middley, r);
	}

	ofSetColor(255,255,255);

	fbo.end();
	if(counter > (next + 150))
	{
	//	cout << "OLD at " << next << endl;
		next = counter;
	//	cout << "NEW at " << next << " NOW at "<< counter << endl;
	}
	
}

ofFbo yellowblock::getfbo()
{

	return fbo;
}