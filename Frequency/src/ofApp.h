#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp
{
	public:
	
		// --------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
		}
	
		// --------------------------------
		void draw()
		{
			ofBackgroundGradient( ofColor(55), ofColor(0), OF_GRADIENT_CIRCULAR );
			
			ofEnableAlphaBlending();
			
			// As we are feeding values measured in pixels, the frequency value will be pretty small
			float frequency = ofMap( ofGetMouseX(),  0, ofGetWidth(),  0.0001, 0.05 );

			// Draw ticks at each unit edge
			int numTicks = ceil(ofGetWidth() * frequency);
			float areaCovered = numTicks;
			if( numTicks <= 5 ) { numTicks *= 10; }
			
			for( int i = 0; i < numTicks; i++ )
			{
				float noisePos = ofMap( i, 0, numTicks, 0, areaCovered);
				float screenX = noisePos * (1.0/frequency);
				ofVec2f top( screenX, 0);
				ofVec2f bot( screenX, ofGetHeight());
			
				// Brighter white lines at unit edges (0,1,2) and less bright at fractionals (0.1,0.2, etc), but only if we are zoomed in enough
				if( (abs(fmodf(noisePos, 1.0)) <= 0) && areaCovered < 18 )  { ofSetColor( ofColor::white, 90 ); } else { ofSetColor( ofColor::white, 30 );  }

				ofLine( top, bot);
				
				ofVec2f textPos = top + ofVec2f(4,20);
				fontSmall.drawString( ofToString(noisePos), textPos.x, textPos.y );
			}
			
			// Compute a noise position for each pixels x position, taking a frequency value into account
			float noiseMagnitude = 300;
			ofMesh mesh;
			mesh.setMode( OF_PRIMITIVE_LINE_STRIP );
			int res = ofGetWidth();
			for( int i = 0; i < res; i++ )
			{
				ofVec2f pos = ofVec2f( ofMap( i, 0, res, 0, ofGetWidth() ), ofGetHeight() * 0.5f );
				
				float noisePos = (pos.x * frequency);
				float noiseValue = ofSignedNoise( noisePos ); // ofSignedNoise give us a value -1..1
				
				ofVec2f drawPos = pos + ofVec2f(0,noiseValue * noiseMagnitude);
				
				mesh.addVertex( drawPos );
			}

			ofSetColor( ofColor::white );
			mesh.draw();
		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )
			{
			}
		}
	
		ofTrueTypeFont fontSmall;
};
