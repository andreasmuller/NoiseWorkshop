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
			
			gui.add( cloudColor.set("Cloud Color", ofColor::white, ofColor(0,0,0,0), ofColor::white ) );

			
			gui.add( numOctaves.set("Num Octaves",			  1,  1,		10) );
			gui.add( baseFrequency.set("Base Frequency",  0.01f,  0.0001f,  0.05f) ); // As we are feeding values measured in pixels, the frequency value will be pretty small
			gui.add( persistence.set("Persistence",		   0.1f,  0.001f,   1.0f) );
			
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
			camera.setFarClip( 512 );
			
			drawGui = true;
		}


		// --------------------------------
		void update()
		{

		}
	
		// --------------------------------
		void draw()
		{
			camera.begin();
			
				ofEnableAlphaBlending();
					
				shader.begin();
				
					shader.setUniform1f("iGlobalTime", ofGetElapsedTimef() );
					shader.setUniform2f("iResolution", ofGetWidth(), ofGetHeight() );
					//shader.setUniform2f("iMouse", ofGetMouseX(), ofGetMouseY() );
					shader.setUniformTexture("iChannel0", noiseImage, 0 );

					float fov_y_scale = tan( ofDegToRad(camera.getFov())/2);
					shader.setUniform1f("fov_y_scale", fov_y_scale );

					shader.setUniform1f("aspect", (float)ofGetWidth() / ofGetHeight() );
	
					ofFloatColor cloudBaseColor = cloudColor.get();
					shader.setUniform4fv("cloudBaseColor", cloudBaseColor.v );
			
					//ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight() );
					ofRect( -1, -1, 2, 2 );
			
				shader.end();

				//ofDisableDepthTest();
				//ofDrawBox( 0.5f );
			
			camera.end();

			
			if( drawGui ) { gui.draw(); }
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
		
		ofParameter<int> numOctaves;
		ofParameter<float> baseFrequency;
		ofParameter<float> persistence;
		ofParameter<ofColor> cloudColor;
	
		bool drawGui;
	
		ofTrueTypeFont fontSmall;
};
