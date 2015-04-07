#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/ofTrueTypeFontExt.h"

// -----------------------------------------------------------
class Particle
{
	public:
	
		ofVec3f pos;
		ofVec3f vel;
		ofVec3f noiseRandomOffset;
		deque<ofVec3f> posHistory;
		float age;
};

// -----------------------------------------------------------
class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void drawParticles();
		void reInitParticle( Particle* _p );
	
		ofVec3f getNoise( ofVec3f _noisePos, float _time );
	
		void numParticlesChanged(int& _param );			// Callbacks from the UI
	
		void keyPressed(int key);
	
		vector<Particle>		particles;
	
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
	
		ofxPanel				gui;
	
		ofParameter<int>		numParticles;
		ofParameter<float>		randomOffset;
		ofParameter<float>		noiseMagnitude;
		ofParameter<float>		spaceFrequency;
		ofParameter<float>		timeFrequency;
		ofParameter<float>		oldVelAmount;
		ofParameter<float>		maxVel;
		ofParameter<float>		maxAge;
		bool					drawGui;
};