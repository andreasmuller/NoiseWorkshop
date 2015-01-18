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
	
		void drawGrass( ofCamera* _camera );

		void computeMesh();
	
		void intParamChanged(int& _param );			// Callbacks from the UI
		void floatParamChanged(float& _param );
	
		void keyPressed(int key);
	
		ofxFirstPersonCamera	camera;
	
		ofVboMesh				grassMesh;
		ofShader				grassShader;
	
		ofTrueTypeFontExt		fontSmall;
	
		ofxPanel				gui;

		ofParameter<int>		placementResolution;
		ofParameter<float>		placementSize;
		ofParameter<float>		placementNoiseSpaceFrequency;
		ofParameter<float>		placementBottomThreshold;
	
		ofParameter<float>		stalkWidth;
		ofParameter<float>		stalkHeight;
		ofParameter<float>		swayingMaxAngle;
		ofParameter<float>		swayingTimeScale;
		ofParameter<float>		swayingNoiseSpaceFrequency;
		ofParameter<float>		swayingTimeMaxDifference;
	
		bool					drawGui;
};