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
			
			string xmlSettingsPath = "Settings/PerlinLayering.xml";
			gui.setup( "Perlin Layers", xmlSettingsPath );
			
			gui.add( numOctaves.set("Num Octaves",			  1,  1,		10) );
			gui.add( baseFrequency.set("Base Frequency",  0.01f,  0.0001f,  0.05f) ); // As we are feeding values measured in pixels, the frequency value will be pretty small
			gui.add( persistence.set("Persistence",		   0.1f,  0.001f,   1.0f) );
			
			gui.loadFromFile( xmlSettingsPath );
			
			drawGui = true;
		}

		// --------------------------------
		void update()
		{
			int res = 256;

			// clear and allocate images if needed
			if( numOctaves != layers.size() ) layers.clear();
	
			while( layers.size() < numOctaves )
			{
				layers.push_back( ofFloatImage() );
				layers.back().allocate( res, res, OF_IMAGE_GRAYSCALE ); // this won't do anything if everything is already set the way we need it
			}
			
			noiseResult.allocate( res, res, OF_IMAGE_GRAYSCALE );



			// Compute noise
			
			ofFloatPixelsRef pix = noiseResult.getPixelsRef();
			
			int tmpIndex = 0;
			for( int y = 0; y < res; y++ )
			{
				for( int x = 0; x < res; x++ )
				{
					float totalAmplitude = 0.0f;
					float finalValue = 0.0f;
					for( int i = 0; i < numOctaves; i++ )
					{
						float frequency = powf(2.0, i) * baseFrequency;
						float amplitude = powf(persistence, i);
						
						totalAmplitude += amplitude; // let's store the total amplitude alue so we can normalize later
						
						float layerValue = ofNoise( x * frequency, y * frequency );
						finalValue += layerValue * amplitude;
						
						layers.at(i).getPixelsRef()[tmpIndex] = layerValue; // store the value in a separate texture so we can have a look at it
					}
					
					pix[tmpIndex] = finalValue / totalAmplitude;
					
					tmpIndex++;
				}
			}
			
			// We've manually changed the pixels, so make sure we uodate
			for( int i = 0; i < numOctaves; i++ ) { layers.at(i).update(); }
			noiseResult.update();
			
		}
	
		// --------------------------------
		void draw()
		{
			float w = ofGetWidth();
			float h = ofGetHeight();
			ofRectangle screenRect(0,0,w,h);
			
			ofBackgroundGradient( ofColor(55), ofColor(0), OF_GRADIENT_CIRCULAR );
			
			ofRectangle resultDrawRect( 0, 0, noiseResult.getWidth(), noiseResult.getHeight() );
			resultDrawRect.scaleTo( screenRect, OF_SCALEMODE_CENTER );
			
			noiseResult.draw( resultDrawRect );
			
			if( numOctaves > 1 )
			{
				float border = 3;
				//float layerDrawSize = MIN( (h - (border * layers.size())) / layers.size(), noiseResult.getHeight() + (border*2) );
				float layerDrawSize = (h - (border * (layers.size()+1))) / layers.size();
				
				for( unsigned int i = 0; i < layers.size(); i++ )
				{
					layers.at(i).draw( w - layerDrawSize - border, ((layerDrawSize+border) * i) + border, layerDrawSize, layerDrawSize );
				}
			}
			
			
			if( drawGui )
			{
				gui.draw();
			}
		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )
			{

			}
			else if( _key == OF_KEY_TAB )
			{
				drawGui = !drawGui;
			}
		}
	
		vector<ofFloatImage> layers; // stores the result of each noise layer computation
		ofFloatImage noiseResult;
	
		ofxPanel gui;
		
		ofParameter<int> numOctaves;
		ofParameter<float> baseFrequency;
		ofParameter<float> persistence;
		
		bool drawGui;
	
		ofTrueTypeFont fontSmall;
};
