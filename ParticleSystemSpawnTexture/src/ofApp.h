#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "ParticleSystemSpawnTexture.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		float					time;
		float					timeStep;
	
		ofVideoPlayer			videoPlayer;
	
		vector<ofVec3f>			spawnPositions;
		ofPixels				maskPixels;
	
		ofMesh					emissionMesh;
		ofMesh					debugNormalsMesh;
	
		ParticleSystemSpawnTexture	particles;
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
	
		bool					drawGui;
};