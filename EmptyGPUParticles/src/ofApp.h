#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "Utils/Cameras/ofxWalkingFirstPersonCamera.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		float					particleMaxAge;
		FboPingPong				particleData;
	
		ofxFirstPersonCamera	camera;
	
		ofMesh					particlesMesh;
	
		ofTrueTypeFontExt		fontSmall;
};