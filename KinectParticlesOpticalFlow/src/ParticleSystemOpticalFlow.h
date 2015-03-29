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

class ParticleSystemOpticalFlow
{

	public:
	
		void init();
		void update( float _time, float _timeStep );
		void draw( ofCamera* _camera );

		void reinitParticles();
		void reinitParticlePosAndAge();
	
		void updateParticles( float _time, float _timeStep );
		void drawParticles( ofShader* _shader, ofCamera* _camera );
	
		void allocateOpticalFlow( int _w, int _h );
		void setWorldToFlowParameters( ofMatrix4x4 _worldToKinect );
		void setAverageFlow( ofVec2f _flow );
	
		ofFloatPixelsRef getOpticalFlowPixels() { return opticalFlowBuffer; }
	
		void drawGui();
	
		float getParticleMaxVel() { return maxVel.get(); }
		float getFlowMaxLength() { return flowMaxLength.get(); }
	
		void floatParamChanged( float& _param );
		void intParamChanged( int& _param );
	
		int						numParticles;
		//int						textureSize;
	
		FboPingPong				particleDataFbo;
	
		ofFloatPixels			opticalFlowBuffer;
		ofTexture				opticalFlowTexture;
		ofVec2f					averageFlow;
	
		ofFloatPixels			spawnPosBuffer;
		ofTexture				spawnPosTexture;
	
		ofVboMesh				singleParticleMesh;

		ofMaterial				particleMaterial;
	
		ofxAutoReloadedShader	particleUpdate;
		ofxAutoReloadedShader	particleDraw;

		ofxAutoReloadedShader	debugDrawOpticalFlow;
	
		ofMatrix4x4				worldToKinect;

		ofxPanel				gui;
		ofParameter<int>		textureSize;
		ofParameter<float>		particleMaxAge;
		ofParameter<float>		startScale;
		ofParameter<float>		endScale;
		ofParameter<float>		maxVel;
		ofParameter<float>		noisePositionScale;
		ofParameter<float>		noiseMagnitude;
		ofParameter<float>		noiseTimeScale;
		ofParameter<float>		noisePersistence;
		ofParameter<float>		oldVelToUse;

		ofParameter<float>		flowMagnitude;
		ofParameter<float>		flowMaxLength;
		ofParameter<float>		averageFlowMagnitude;
	
//		ofParameter<float>		twistNoiseTimeScale;
//		ofParameter<float>		twistNoisePosScale;
//		ofParameter<float>		twistMinAng;
//		ofParameter<float>		twistMaxAng;
		
		ofParameter<ofVec3f>	baseSpeed;
		
		ofParameter<ofColor>	startColor;
		ofParameter<ofColor>	endColor;
	
		ofxPanel				guiMaterial;

		//ofParameter<ofColor>	materialDiffuse; // We will provide our own diffuse per particle
		ofParameter<ofColor>	materialAmbient;
		ofParameter<ofColor>	materialSpecular;
		ofParameter<ofColor>	materialEmissive;
	
		ofParameter<float>		materialShininess;

};
