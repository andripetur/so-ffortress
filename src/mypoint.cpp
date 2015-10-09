#include "mypoint.h"


mypoint::mypoint(float nx, float ny, float nangle, float nlength)
{
	x = nx;
	y = ny;
	length = nlength;
	anglerad = nangle;
}


mypoint::~mypoint(void)
{
}

ofPoint mypoint::getofPoint()
{
	return ofPoint(x, y);
}