#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

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
	
		void updateParticles();
		void drawParticles();

		void keyPressed(int key);
	
		float					time;
		float					timeStep;
	
		int						numParticles;
		int						textureSize;
	
		ofxFirstPersonCamera	camera;
	
		FboPingPong				particleDataFbo;
	
		ofVboMesh				particlePoints;
	
		ofxAutoReloadedShader	particleUpdate;
		ofxAutoReloadedShader	particleDrawUnsorted;

		ofImage					particleImage;

		ofTrueTypeFontExt		fontSmall;

		ofxPanel				gui;
		ofParameter<float>		particleMaxAge;
		ofParameter<float>		noisePositionScale;
		ofParameter<float>		noiseMagnitude;
		ofParameter<float>		noiseTimeScale;
		ofParameter<float>		noisePersistence;
		ofParameter<float>		twistNoiseTimeScale;
		ofParameter<float>		twistNoisePosScale;
		ofParameter<float>		twistMinAng;
		ofParameter<float>		twistMaxAng;
	
		ofParameter<ofVec3f>	baseSpeed;

		ofParameter<ofColor>	startColor;
		ofParameter<ofColor>	endColor;
	
		ofParameter<float>		particleSize;
	
		bool					drawGui;
};