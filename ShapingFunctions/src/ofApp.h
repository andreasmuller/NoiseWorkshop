#pragma once

#include "ofMain.h"
#include "Math/MathUtils.h"

class ofApp : public ofBaseApp
{
	public:
	
		void setup();
		void update();
		void draw();
	
		float getTerrain( float _t, float _magnitude );
		
		float time;

};
