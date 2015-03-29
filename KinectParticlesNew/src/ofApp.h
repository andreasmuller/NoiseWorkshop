
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FileUtils.h"
#include "Utils/Cameras/ofxFirstPersonCamera.h"

#include "Math/MathUtils.h"

#include "ParticleSystemOpticalFlow.h"
//#include "ParticleSystemSpawnTexture.h"


#include "KinectManager.h"

#define APP_MODE_LIVE				0
#define APP_MODE_TOUCH_SETUP		1
#define APP_MODE_AMOUNT				2

class ofApp : public ofBaseApp
{
	public:

		void setup();
		void update();
		void draw();
		void exit();

		void drawScene();
		void debugDrawOpticalFlow();
	
		void keyPressed  (int key);
		void mouseMoved(int x, int y );

		int						currAppMode;
	
		ParticleSystemOpticalFlow	particles;
		//ParticleSystemSpawnTexture particlesLight;
		ofxFirstPersonCamera	camera;

		ofxAutoReloadedShader	debugOpticalFlowShader;
	
		KinectManager			kinectManager;

		ofMatrix4x4				kinectToWorld;
		ofMatrix4x4				worldToKinect;
	
		float					time;
		float					timeStep;

		ofTrueTypeFontExt 		fontSmall;
	
		ofMesh					pointCloudMesh;
		vector<ofVec3f>			pointCloudPoints;
		vector<ofFloatColor>	pointCloudColors;
	
		ofParameter<ofColor>	globalAmbient;

		ofParameter<ofVec3f>	light1Position;
		ofParameter<ofColor>	light1Ambient;
		ofParameter<ofColor>	light1Diffuse;
		ofParameter<ofColor>	light1Specular;

		ofParameter<bool>		drawPointCloud;

		ofParameter<ofVec3f>	testPos;
	
		float					lastTimeMouseMoved;
	
		ofxPanel				gui;

		ofParameter<float>		kinectPointCloudScale;
		ofParameter<ofVec3f>	kinectPointCloudOffset;

		ofParameter<bool>		debugFlow;
		ofParameter<ofVec3f>	debugFlowPos;
		ofParameter<ofVec2f>	debugFlowSize;
		ofParameter<int>		debugFlowRes;
	
		ofVec2f					testDepthImagePos;
	
		ofLight					light[1];
	
		bool					drawUI;
};


