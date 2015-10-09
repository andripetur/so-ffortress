#include "treefractal.h"
#include <math.h>


treefractal::treefractal()
{
	
}

void treefractal::setup(float astartx, float astarty)
{
	startx = astartx;
	starty = astarty;
}


treefractal::~treefractal(void)
{
}


void treefractal::draw(float branchlength, float angle, float numiterations, float lengthmultiplier, float startangle, int randomseed, float randomness) //all angles in degrees!
{
	/*
	float branchlength = 20;
	float angle = 30 * 0.017453292519943295;
	int numiterations = 10;
	float lengthmultiplier = 0.9;
	float startangle = 70; //in degrees!
	*/

	ofSeedRandom(randomseed);


	angle = angle * 0.017453292519943295; //convert degrees to rad

	ofSetColor(0, 255, 0);
	//Draworigin
	//ofPoint puntje(40, 40);
	

	//Drawbase
	float startpointx = randomness * (ofRandom(branchlength) - (0.5 * branchlength)) + startx + branchlength * sin(startangle * 0.017453292519943295);
	float startpointy = randomness * (ofRandom(branchlength) - (0.5 * branchlength)) + starty + branchlength * cos(startangle * 0.017453292519943295);


	mypoint startpoint(startpointx, startpointy, startangle * 0.017453292519943295, branchlength);

	ofLine(ofPoint(startx, starty), startpoint.getofPoint());

	//drawbranches
	vector<mypoint> startpoints;

	vector<mypoint> endpoints;

	startpoints.push_back(startpoint);

	int numfulliterations = int(numiterations);
	float rest = numiterations - numfulliterations;
    
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	for (int j = 0; j < numfulliterations + 1; j++)
	{
		for (int i = 0; i < startpoints.size(); i++)
		{
			//drawR
			float newlength;
			newlength = startpoints[i].length * lengthmultiplier;


			float lastangle = startpoints[i].anglerad;
			float thisangle = angle + lastangle;
			float temprandom = randomness * (ofRandom(newlength) - (0.5 * newlength));

			float newlengthR = newlength + temprandom;

			if(j>= numfulliterations){
				newlengthR = newlengthR * rest;
			}


			float endx = startpoints[i].x + (newlengthR * sin(thisangle));
			float endy = startpoints[i].y + (newlengthR * cos(thisangle));

			endpoints.push_back(mypoint(endx, endy, thisangle, newlengthR));
			ofLine(startpoints[i].getofPoint(), ofPoint(endx, endy));
			
			//drawL
			
			float thisangleL = lastangle - angle;
			temprandom = randomness * (ofRandom(newlength) - (0.5 * newlength));
			
			float newlengthL = newlength + temprandom;

			if(j>= numfulliterations){
				newlengthL = newlengthL * rest;
			}


			float endxL = startpoints[i].x + (newlengthL * sin(thisangleL));
			float endyL = startpoints[i].y + (newlengthL * cos(thisangleL));

			endpoints.push_back(mypoint(endxL, endyL, thisangleL, newlengthL));
			ofLine(startpoints[i].getofPoint(), ofPoint(endxL, endyL));
			

		}
		startpoints = endpoints;
		endpoints.clear();
	}


	/*

	//drawR
	float endx = newpoint.x + (branchlength * sin(angle));
	float endy = newpoint.y + (branchlength * cos(angle));

	
	ofPoint newtopointR(endx, endy);
	ofLine(newpoint, newtopointR);

	//drawL
	float endxL = newpoint.x + (branchlength * sin(-angle));
	float endyL = newpoint.y + (branchlength * cos(-angle));

	
	ofPoint newtopointL(endxL, endyL);
	ofLine(newpoint, newtopointL);

	*/


}