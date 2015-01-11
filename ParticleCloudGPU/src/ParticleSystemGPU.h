//
//  ParticleSystemGPU.h
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"
#include "Utils/FboPingPong.h"

class ParticleSystemGPU
{

	public:
	
		void init( int _texSize );
		void update( float _time, float _timeStep );
		void draw( ofCamera* _camera );
	
		int						numParticles;
		int						textureSize;
	
		FboPingPong				particleDataFbo;
		
		ofVboMesh				particlePoints;
		
		ofxAutoReloadedShader	particleUpdate;
		ofxAutoReloadedShader	particleDrawUnsorted;
		
		ofImage					particleImage;

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
	
};
