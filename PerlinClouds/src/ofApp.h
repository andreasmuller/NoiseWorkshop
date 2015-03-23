#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
	public:
	
	
	// Fractal brownian motion
	
		// --------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
			
			ofxGuiSetFont( "Fonts/DIN.otf", 8 );
			ofxGuiSetDefaultWidth( 260 );
			
			string xmlSettingsPath = "Settings/PerlinClouds.xml";
			gui.setup( "Perlin Clouds", xmlSettingsPath );

			gui.add( frequency.set("Frequency",  1.0f,  0.01f,  2.0f) );
			gui.add( cloudColor.set("Cloud Color", ofColor::white, ofColor(0,0,0,0), ofColor::white ) );

			gui.loadFromFile( xmlSettingsPath );
			
			
			shader.load("Shaders/RayMarchingCloudsVolumeofCamera");
			
			ofDisableArbTex();
				noiseImage.loadImage("Textures/NoiseForClouds.png");
				noiseImage.getTextureReference().setTextureWrap( GL_REPEAT, GL_REPEAT );
			ofEnableArbTex();
			
			camera.setAutoDistance( false );
			camera.setGlobalPosition( ofVec3f(0.1,2,-3.5) );
			camera.lookAt( ofVec3f(0,0,0) );
			camera.setNearClip( 0.0001f );
			camera.setFarClip( 1024 );
			
			drawGui = true;
		}


		// --------------------------------
		void update()
		{

		}
	
		// --------------------------------
		void draw()
		{
			ofSetColor( ofColor::white );
			
			camera.begin();
			
				ofEnableAlphaBlending();
					
				shader.begin();

					shader.setUniform1f("aspect", (float)ofGetWidth() / ofGetHeight() );
					shader.setUniform1f("fovYScale", tan( ofDegToRad(camera.getFov())/2 ) );
			
					shader.setUniform1f("time", ofGetElapsedTimef() );
					shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight() );
					shader.setUniformTexture("noiseTexture", noiseImage, 0 );
					shader.setUniform1f("frequency", frequency );
			
					ofFloatColor cloudBaseColor = cloudColor.get();
					shader.setUniform4fv("cloudBaseColor", cloudBaseColor.v );
			
					ofRect( -1, -1, 2, 2 );
			
				shader.end();

				//ofDisableDepthTest();
				//ofDrawBox( 0.5f );
			
			camera.end();

			
			if( drawGui ) { gui.draw(); }
			
			ofSetColor( ofColor::black );
			fontSmall.drawString( ofToString(ofGetFrameRate(), 1), ofGetWidth() - 25, ofGetHeight() - 5 );
		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )			  { }
			else if( _key == OF_KEY_TAB ) { drawGui = !drawGui; }
			else if( _key == 'f' )		  { ofToggleFullscreen(); }
		
		}

		ofEasyCam camera;
		ofShader shader;
		ofImage noiseImage;
	
	
		ofxPanel gui;
	
		ofParameter<float> frequency;
		ofParameter<ofColor> cloudColor;
	
		bool drawGui;
	
		ofTrueTypeFont fontSmall;
};
