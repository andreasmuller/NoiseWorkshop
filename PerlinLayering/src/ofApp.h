#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
	public:
	
		// --------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
			
			
			ofxGuiSetFont( "Fonts/DIN.otf", 8 );
			ofxGuiSetDefaultWidth( 260 );
			
			string xmlSettingsPath = "Settings/PerlinLayering.xml";
			gui.setup( "Perlin Layers", xmlSettingsPath );
			
			gui.add( numOctaves.set("Num Octaves",					1,  1,  10) );
			gui.add( baseFrequency.set("Base Frequency",		    0.01f,  0.001f,  0.2f) );
			gui.add( persistence.set("Persistence",					2.0f,  0.001f,  0.2f) );
			
			gui.loadFromFile( xmlSettingsPath );
			
			drawGui = true;
		}

		// --------------------------------
		void update()
		{
			// As we are feeding values measured in pixels, the frequency value will be pretty small
			//float frequency = ofMap( ofGetMouseX(),  0, ofGetWidth(),  0.0001, 0.05 );
			
			int res = 256;

			// clear and allocate images if needed
			if( numOctaves != layers.size() ) layers.clear();
	
			while( layers.size() < numOctaves )
			{
				layers.push_back( ofFloatImage() );
				layers.back().allocate( res, res, OF_IMAGE_GRAYSCALE ); // this won't do anything if everything is already set the way we need it
			}
			
			noiseResult.allocate( res, res, OF_IMAGE_GRAYSCALE );

			ofFloatPixelsRef pix = noiseResult.getPixelsRef();

			// Compute noise
			int tmpIndex = 0;
			for( int y = 0; y < res; y++ )
			{
				for( int x = 0; x < res; x++ )
				{
					float total = 0.0f;
					for( int i = 0; i < numOctaves; i++ )
					{
						//float frequency = (i * 2) * baseFrequency;
						float frequency = powf(2.0, i) * baseFrequency;
						float amplitude = powf(persistence, i); //p * i;
						
						float layerVal = ofNoise( x * frequency, y * frequency );
						total += layerVal * amplitude;
						
						layers.at(i).getPixelsRef()[tmpIndex] = layerVal;
					}
					
					pix[tmpIndex] = total;
					
					/*
					  total = 0
					  p = persistence
					  n = Number_Of_Octaves - 1

					  loop i from 0 to n

						  frequency = 2i
						  amplitude = pi

						  total = total + InterpolatedNoisei(x * frequency, y * frequency) * amplitude

					  end of i loop

					  return total
					 */
					
					tmpIndex++;
				}
			}
			
			// We've manually changed the pixels, so make sure we uodate
			//for( int i = 0; i < numOctaves; i++ ) { layers.at(i).update(); }
			noiseResult.update();
			
		}
	
		// --------------------------------
		void draw()
		{
			ofBackgroundGradient( ofColor(55), ofColor(0), OF_GRADIENT_CIRCULAR );
			
			ofEnableAlphaBlending();
			
			noiseResult.draw(0,0);
			
			//cout << noiseResult.getWidth() << ", " << noiseResult.getHeight() << endl;
			
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
