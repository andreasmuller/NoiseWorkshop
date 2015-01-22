#pragma once

#include "ofMain.h"
#include "ofxGui.h"
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
			
		
			string settingsPath = "Settings/Main.xml";
			gui.setup( "Main", settingsPath );
			
			gui.add( color1.set("Color 1", ofColor::white, ofColor(0,0,0,0), ofColor::white ) );
			gui.add( color2.set("Color 2", ofColor::white, ofColor(0,0,0,0), ofColor::white ) );
			gui.add( color3.set("Color 3", ofColor::white, ofColor(0,0,0,0), ofColor::white ) );
			gui.add( shininess.set("shininess",  20.0f,  0.0f,  127.0f) );
			
			gui.loadFromFile( settingsPath );
			
			drawGui = false;
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

						// Convert our 0..255 colors to 0..1
						ofFloatColor col1 = color1.get();
						ofFloatColor col2 = color2.get();
						ofFloatColor col3 = color3.get();
			
						shader.setUniform4fv("color1", col1.v );
						shader.setUniform4fv("color2", col2.v );
						shader.setUniform4fv("color3", col3.v );

						shader.setUniform1f("shininess", shininess );
			
						ofSetColor( ofColor::white );
						ofDrawSphere( 1.0 );

					shader.end();
			
				material.end();

				ofDisableLighting();
				ofSetColor( light.getDiffuseColor() );
				ofDrawSphere( light.getPosition(), 0.1 );
			
			camera.end();
			
			ofDisableDepthTest();
			
			if( drawGui )
			{
				gui.draw();
			}

		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == OF_KEY_TAB )
			{
				drawGui = !drawGui;
			}
			else if( _key == 'f' )
			{
				ofToggleFullscreen();
			}
		}
	
		ofTrueTypeFont			fontSmall;
	
		ofxFirstPersonCamera	camera;
		ofLight					light;
		ofMaterial				material;

		ofxAutoReloadedShader	shader;
	
	
		ofxPanel				gui;
	
		ofParameter<ofColor>	color1;
		ofParameter<ofColor>	color2;
		ofParameter<ofColor>	color3;
		
		ofParameter<float>		shininess;
	
		bool					drawGui;
	

};
