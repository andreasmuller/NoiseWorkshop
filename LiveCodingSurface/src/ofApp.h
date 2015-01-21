#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "Utils/Cameras/ofxFirstPersonCamera.h"

class ofApp : public ofBaseApp
{
	public:
	
		// --------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
			
			shader.load("Shaders/LiveCodingSurface/GL2/LiveCodingSurface");
			
			//cout << "ico sphere res " << ofGetIcoSphereResolution() << endl;
			//ofSetIcoSphereResolution( 4 );
			ofSetSphereResolution( 60 );
			
			ofDisableArbTex(); // We want our texture coordinates in 0..1
			
			camera.setNearClip(0.01f);
			camera.setPosition( 0, 0, 3 );
			camera.lookAt( ofVec3f( 0, 0, 0) );
			camera.setMovementMaxSpeed( 0.05f );
			
			ofSetSmoothLighting( true );
			light.setPointLight();
			light.setPosition( ofVec3f(2,2,2) );
			light.enable();
			
			ofSetGlobalAmbientColor( ofColor(20,20,20) );
			material.setDiffuseColor( ofFloatColor::white );
			material.setSpecularColor( ofFloatColor::white );
			material.setEmissiveColor( ofFloatColor::black );
			material.setAmbientColor( ofFloatColor::black );
			material.setShininess( 10 );
		}
	
		// --------------------------------
		void draw()
		{
			
			
			ofBackgroundGradient( ofColor(200,220,220), ofColor::white, OF_GRADIENT_CIRCULAR );
			
			float mx = ofGetMouseX() / (float)ofGetWidth();
			float my = ofGetMouseY() / (float)ofGetHeight();

			ofEnableDepthTest();
			light.enable();
			
			//glEnable( GL_CULL_FACE );
			//glCullFace( GL_FRONT );
			
			camera.begin();
			
				material.begin();

					shader.begin();

						shader.setUniform1f("time", ofGetElapsedTimef() );
			
						shader.setUniform1f("mouseX", mx );
						shader.setUniform1f("mouseY", my );
			
						ofSetColor( ofColor::white );
						ofDrawSphere( 1.0 );

					shader.end();
			
				material.end();

				ofDisableLighting();
				ofSetColor( light.getDiffuseColor() );
				ofDrawSphere( light.getPosition(), 0.1 );
			
			camera.end();

		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )
			{
			}
		}
	
		ofTrueTypeFont			fontSmall;
	
		ofxFirstPersonCamera	camera;
		ofLight					light;
		ofMaterial				material;
	
		ofxAutoReloadedShader	shader;
};
