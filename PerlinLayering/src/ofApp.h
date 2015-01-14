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
		void allocateIfNeeded( int _res )
		{
			// Clear and allocate images if needed
			if( numOctaves != layers.size() ) layers.clear();
			
			while( layers.size() < numOctaves )
			{
				layers.push_back( ofFloatImage() );					// allocate float images, this way we can pop in our data in the range 0..1 and draw it
				layers.back().allocate( _res, _res, OF_IMAGE_GRAYSCALE );
			}
			
			noiseResult.allocate( _res, _res, OF_IMAGE_GRAYSCALE ); // this won't do anything if everything is already set the way we need it, so will be fast
		}

		// --------------------------------
		void update()
		{
			int res = 256;
			
			allocateIfNeeded( res );
			
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
						float frequency = powf(2.0, i) * baseFrequency;				//
						float amplitude = powf(persistence, i);						//
						
						totalAmplitude += amplitude;								// let's store the total amplitude so we can normalize later, stupid to do it here but let's keep things simple
						
						float layerValue = ofNoise( x * frequency, y * frequency ); // Lets read the noise value at our location, try adding an offset with the line below
						//float layerValue = ofNoise( ((x + ofGetMouseX()) * frequency), ((y + ofGetMouseY()) * frequency) );
						
						finalValue += layerValue * amplitude;						// Add it to our final result
						
						layers.at(i).getPixelsRef()[tmpIndex] = layerValue;			// store the value in a separate texture so we can have a look at it
					}
					
					pix[tmpIndex] = finalValue / totalAmplitude;
					
					tmpIndex++;
				}
			}
			
			// We've manually changed the pixels in the images, so make sure we update the textures
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
				// Draw the layer results we saved earlier
				vector<ofRectangle> layerDrawRects;
				
				float border = 3;
				float layerDrawSize = (h - (border * (layers.size()+1))) / layers.size();
				layerDrawSize = MIN( layerDrawSize, noiseResult.getWidth() * 0.7 ); // make sure they are always drawn smaller than our result
				float allLayersHeight = (layerDrawSize + border) * layers.size();
				float startY = (ofGetHeight() - allLayersHeight) * 0.5f;
				for( unsigned int i = 0; i < layers.size(); i++ )
				{
					layerDrawRects.push_back( ofRectangle(w - layerDrawSize - border, startY + (((layerDrawSize+border) * i) + border), layerDrawSize, layerDrawSize) );
				}
			
				ofVec2f connectLineStartPoint( resultDrawRect.getCenter() + ofVec2f(resultDrawRect.width * 0.55,0) );
				
				for( unsigned int i = 0; i < layers.size(); i++ )
				{
					layers.at(i).draw( layerDrawRects.at(i) );
					ofVec2f endPoint = layerDrawRects.at(i).getCenter() + ofVec2f(layerDrawRects.at(i).width * -0.55, 0);
					
					smoothStepCurve( connectLineStartPoint, endPoint );
				}
			}
			
			if( drawGui ) { gui.draw(); }
		}
	
		// --------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )			  { }
			else if( _key == OF_KEY_TAB ) { drawGui = !drawGui; }
			else if( _key == 'f' )		  { ofToggleFullscreen(); }
		
		}

		// --------------------------------
		void smoothStepCurve( ofVec2f _start, ofVec2f _end, int _res = 30 )
		{

			int curveRes = 30;
			ofMesh tmpMesh;
			tmpMesh.setMode( OF_PRIMITIVE_LINE_STRIP );
			for( int k = 0; k < curveRes; k++ )
			{
				ofVec2f tmpPoint = _start.getInterpolated( _end, k / (float)(curveRes-1) );
				tmpPoint.y = ofMap( smoothstep(_start.x, _end.x, tmpPoint.x), 0, 1, _start.y, _end.y );
				tmpMesh.addVertex( tmpPoint );
			}
			tmpMesh.draw();
		}
	
		// --------------------------------
		float smoothstep(float edge0, float edge1, float x)
		{
			x = ofClamp( (x - edge0)/(edge1 - edge0), 0, 1); // Scale, and clamp x to 0..1 range
			return x*x*x*(x*(x*6 - 15) + 10); // Evaluate polynomial
		}
		
		// --------------------------------
		float smoothStepInOut( float _t, float _low0, float _high0, float _high1, float _low1 )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			float localLow1  = _low1;
			float localHigh1 = _high1;
			return smoothstep( localLow0, localHigh0, _t ) * (1.0f - smoothstep( localHigh1, localLow1, _t ));
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
