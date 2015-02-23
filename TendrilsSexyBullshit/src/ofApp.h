#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "ofMaterialExt.h"
#include "ofLightExt.h"
#include "ShadowMapLight.h"

#define SHADOW_MAP_TEX_UNIT (3)

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();
	
		void drawTendrils( float _time, ofShader* _shader );
		void computeMesh();
		
		void intParamChanged(int& _param );			// Callbacks from the UI
		void floatParamChanged(float& _param );
	
		void drawScene( float _time, bool _forShadowMap = false );
	
		void keyPressed(int key);

		ofxFirstPersonCamera	camera;
	
		ofMaterialExt			material;
		vector<ShadowMapLight*>	lights;
	
	
		ofVboMesh				roomMesh;
		ofMesh					spheresMesh;
	
		ofVboMesh				tendrilMesh;
	
		ofShader				tendrilShaderSaveLinearDepth;
		ofShader				tendrilShader;
	
		ofxAutoReloadedShader	lightingShader;

		ofTrueTypeFontExt		fontSmall;
		ofTrueTypeFontExt		fontMedium;
		ofTrueTypeFontExt		fontLarge;

		ofxPanel				gui;

		ofParameter<ofColor>	sceneAmbient;
	
		ofParameter<ofColor>	lightDiffuse1;
		ofParameter<ofColor>	lightDiffuse2;
		ofParameter<ofColor>	lightDiffuse3;
		ofParameter<ofColor>	lightDiffuse4;

		ofParameter<float>		lightRadius1;
		ofParameter<float>		lightRadius2;
		ofParameter<float>		lightRadius3;
		ofParameter<float>		lightRadius4;

	
		ofxPanel				guiShape;
		
		ofParameter<int>		placementResolution;
		ofParameter<float>		placementSize;
		ofParameter<float>		placementNoiseSpaceFrequency;
		ofParameter<float>		placementBottomThreshold;
		
		ofParameter<float>		tendrilRadius;
		ofParameter<float>		tendrilHeight;
		ofParameter<float>		swayingMaxAngle;
		ofParameter<float>		swayingTimeScale;
		ofParameter<float>		swayingNoiseSpaceFrequency;
		ofParameter<float>		swayingTimeMaxDifference;
		

		ofxPanel				guiMaterial;
	
		ofParameter<ofColor>	materialDiffuse;
		ofParameter<ofColor>	materialSpecular;
		ofParameter<float>		materialShininess;

		ofxPanel				guiShadowMap;
	
		ofParameter<float>		shadowMapLightFov;
		ofParameter<float>		shadowMapLightNear;
		ofParameter<float>		shadowMapLightFar;
		ofParameter<float>		shadowMapLightBlurFactor;
		ofParameter<int>		shadowMapLightBlurNumPasses;
		ofParameter<float>		shadowCoeffecient;
	

		bool					drawGui;
};