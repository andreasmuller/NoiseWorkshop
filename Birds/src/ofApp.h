#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();
	
		void drawBirds();
		void drawBird( float _flapTime, float _flapMagnitude, float _extraFlapAngle );
		ofVec3f getBirdPosNoise( ofVec3f _noisePos, float _time );
	
		void intParamChanged(int& _param );			// Callbacks from the UI
		void floatParamChanged(float& _param );
	
		void keyPressed(int key);
	
		vector<float>			birdFlapTime;
		ofVboMesh				wingMesh;
	
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
	
		ofxPanel				gui;

		ofParameter<int>		placementResolution;
		ofParameter<float>		placementSize;
		ofParameter<float>		placementNoiseSpaceFrequency;
		ofParameter<float>		placementBottomThreshold;
	
		bool					drawGui;
};