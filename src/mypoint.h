#pragma once
#include "ofMain.h"

class mypoint
{
public:
	mypoint(float x, float y, float angle, float length);
	~mypoint(void);
	float x;
	float y;
	float anglerad;
	float length;

	ofPoint getofPoint();

};

