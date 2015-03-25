
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FileUtils.h"
#include "Utils/Cameras/ofxFirstPersonCamera.h"

#include "Math/MathUtils.h"

#include "ParticleSystemInstancedGeometrySpawnTexture.h"
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
	
		void keyPressed  (int key);
		void mouseMoved(int x, int y );

		int						currAppMode;
	
		ParticleSystemInstancedGeometrySpawnTexture	particlesGeometry;
		//ParticleSystemSpawnTexture particlesLight;
		ofxFirstPersonCamera	camera;
	
		KinectManager			kinectManager;

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

		float					lastTimeMouseMoved;
	
		ofxPanel				gui;

		ofParameter<float>		kinectPointCloudScale;
		ofParameter<ofVec3f>	kinectPointCloudOffset;

	
		ofLight					light[1];
	
		bool					drawUI;
};


