#include "blueblock.h"
#include <math.h>


blueblock::blueblock()
{
	
}

void blueblock::setup(int drawwidth, int drawheight)
{
    img.allocate(drawwidth, drawheight, OF_IMAGE_COLOR);
	nextflash = 0;
	nextflashlength = 0;
    drawWidth = drawwidth;
    drawHeight = drawheight;
}


blueblock::~blueblock(void)
{
}


void blueblock::update(float x, float y, int randomness, int length, float speed, int counter)
{

	if(counter >= nextflash && counter < (nextflash + nextflashlength)){		
		//blueimage.clear();

		for (int tx = 0; tx < drawWidth; tx++)
		{
			for (int ty = 0; ty < drawHeight; ty++)
			{
				img.setColor(tx, ty, ofColor(ofRandom(255), ofRandom(255),ofRandom(255)));
			}
		}


		ofSetColor(255, 255, 255);
		//ofRect(x, y, drawWidth, drawHeight);

		//cout << "hit " << x << " , " << y << "width " << drawWidth << " height " << drawHeight;
	}
	else{
		ofSetColor(255, 255, 255);
		img.setColor(ofColor(0, 0, 0));


	}


	if ((nextflash + nextflashlength) < counter)
	{
		nextflash = counter + speed + ofRandom(randomness);

		if (speed <= length){
			nextflashlength = max(max(length + ofRandom(randomness / 2), (float)0.25 * speed), (float)1);
		}
		else{
			nextflashlength = length+ ofRandom(randomness / 2);
		}
		//cout << "counter: " << counter << " next: "<< nextflash << endl;

	}
	//img.update();
}

ofImage blueblock::getimage()
{
	return img;



}