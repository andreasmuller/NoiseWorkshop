#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
	public:
	
		// -----------------------------------------------------------------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
			
			ofxGuiSetFont( "Fonts/DIN.otf", 8 );
			ofxGuiSetDefaultWidth( 260 );
			
			string xmlSettingsPath = "Settings/PerlinLayering.xml";
			gui.setup( "Perlin Layers", xmlSettingsPath );
			
			gui.add( numOctaves.set("Num Octaves",			  1,  1,		10) );
			gui.add( frequency.set("Frequency",  0.01f,  0.0001f,  0.05f) ); // As we are feeding values measured in pixels, the frequency value will be pretty small
			gui.add( lacunarity.set("Lacunarity",		   2.0f,  0.001f,   10.0f) );
			gui.add( persistence.set("Persistence",		   0.1f,  0.001f,   1.0f) );
			
			gui.loadFromFile( xmlSettingsPath );
			
			noiseStartX = 0;
			noiseStartY = 0;
			
			drawGui = true;
		}

		// -----------------------------------------------------------------------------------------
		void allocateIfNeeded( int _res )
		{
			// Clear and allocate images if needed
			if( numOctaves != layers.size() )
			{
				layers.clear();
				layerAmplitude.clear();
			}
			
			while( layers.size() < numOctaves )
			{
				layers.push_back( ofFloatImage() );	// allocate float images, this way we can pop in our data in the range 0..1 and draw it
				layers.back().allocate( _res, _res, OF_IMAGE_GRAYSCALE );
				layerAmplitude.push_back( 0 );
			}
			
			noiseResult.allocate( _res, _res, OF_IMAGE_GRAYSCALE ); // this won't allocate again if everything is already set the way we need it, so will be fast
		}

		// -----------------------------------------------------------------------------------------
		void update()
		{
			int res = 256;
			
			allocateIfNeeded( res );
			
			if( ofGetKeyPressed(OF_KEY_LEFT)  )  { noiseStartX += 5; }
			if( ofGetKeyPressed(OF_KEY_RIGHT ) ) { noiseStartX -= 5; }
			if( ofGetKeyPressed(OF_KEY_UP)  )    { noiseStartY += 5; }
			if( ofGetKeyPressed(OF_KEY_DOWN ) )  { noiseStartY -= 5; }
			
			// Compute noise
			ofFloatPixelsRef pix = noiseResult.getPixelsRef();
			int tmpIndex = 0;
			for( int y = 0; y < res; y++ )
			{
				for( int x = 0; x < res; x++ )
				{
					float noiseValue = fbm( ofVec2f( noiseStartX + x, noiseStartY + y) * frequency, numOctaves, lacunarity, persistence, tmpIndex );
					pix[tmpIndex] = noiseValue;
					tmpIndex++;
				}
			}
			
			// We've manually changed the pixels in the images, so make sure we update the textures
			for( int i = 0; i < numOctaves; i++ ) { layers.at(i).update(); }
			noiseResult.update();
		}
	
		// -----------------------------------------------------------------------------------------
		void draw()
		{
			float w = ofGetWidth();
			float h = ofGetHeight();
			ofRectangle screenRect(0,0,w,h);
			
			ofBackgroundGradient( ofColor(55), ofColor(0), OF_GRADIENT_CIRCULAR );
			
			ofEnableBlendMode( OF_BLENDMODE_ALPHA );
			ofSetColor( ofColor::white );
			
			ofRectangle resultDrawRect( 0, 0, noiseResult.getWidth(), noiseResult.getHeight() );
			resultDrawRect.scaleTo( screenRect, OF_SCALEMODE_CENTER );
			
			noiseResult.draw( resultDrawRect );
			
			if( numOctaves > 1 )
			{
				// Draw the layer results we saved earlier
				vector<ofRectangle> layerDrawRects;
				
				float border = 6;
				float layerDrawSize = (h - (border * (layers.size()+1))) / layers.size();
				layerDrawSize = MIN( layerDrawSize, noiseResult.getWidth() * 0.7 ); // make sure they are always drawn smaller than our result
				float allLayersHeight = (layerDrawSize + border) * layers.size();
				float startY = (ofGetHeight() - allLayersHeight) * 0.5f;
				for( unsigned int i = 0; i < layers.size(); i++ )
				{
					layerDrawRects.push_back( ofRectangle(w - layerDrawSize - border, (startY + (((layerDrawSize+border) * i) + border))-(border/2), layerDrawSize, layerDrawSize) );
				}
			
				ofVec2f connectLineStartPoint( ofVec2f(resultDrawRect.getCenter()) + ofVec2f(resultDrawRect.width * 0.55,0) );
				
				// Draw the layers and a curve connecting the individual layers and the result
				for( unsigned int i = 0; i < layers.size(); i++ )
				{
					layers.at(i).draw( layerDrawRects.at(i) );
					
					ofVec2f endPoint = ofVec2f(layerDrawRects.at(i).getCenter()) + ofVec2f(layerDrawRects.at(i).width * -0.88, 0);
					smoothStepCurve( connectLineStartPoint, endPoint );
				}
				
				// Show the amount each layer is mixed in
				for( unsigned int i = 0; i < layerAmplitude.size(); i++ )
				{
					ofVec2f tmpMiddle = ofVec2f(layerDrawRects.at(i).getCenter()) + ofVec2f(layerDrawRects.at(i).width * -0.69, 0);
					
					int size = 35; //MIN( 35, layerDrawRects.at(i).width / 3 );
					ofRectangle tmpRect;
					tmpRect.setFromCenter(tmpMiddle, size, size );

					ofSetColor( ofColor::black );
					ofFill();
					ofRectRounded( tmpRect, size / 4 );
					
					ofSetColor( ofColor::white );
					ofNoFill();
					ofRectRounded( tmpRect, size / 4 );
					
					ofSetColor( ofColor::white, (int)(layerAmplitude.at(i) * 255) );
					ofFill();
					ofRectRounded( tmpRect, size / 4 );

					string percentString = ofToString( layerAmplitude.at(i) * 100, 1 ) + "%";
					ofVec2f textPos = ofVec2f(tmpRect.getCenter()) + ofVec2f( fontSmall.stringWidth(percentString)*-0.5, fontSmall.stringHeight("H")*0.5 ); // ofVec2f( tmpRect.x, tmpRect.y );

					ofSetColor( ofColor::black ); fontSmall.drawString( percentString, textPos.x+1, textPos.y+1 );
					ofSetColor( ofColor::white ); fontSmall.drawString( percentString, textPos.x,   textPos.y   );
				}

				ofSetColor( ofColor::white );
			}
			
			// draw a side on view showing the result in the middle strip
			ofRectangle sideOnRect( resultDrawRect.x, resultDrawRect.y + resultDrawRect.height, resultDrawRect.width, (h - resultDrawRect.height) * 0.2 );
			//sideOnRect.y += (h - resultDrawRect.height) + sideOnRect.height;
			ofNoFill();
			ofRect( sideOnRect );
			ofFill();
			int nW = noiseResult.getWidth();
			int nH = noiseResult.getHeight();
			ofMesh mesh;
			mesh.setMode( OF_PRIMITIVE_LINE_STRIP );
			for( int i = 0; i < noiseResult.getWidth(); i++ )
			{
				float val = noiseResult.getPixels()[ (((nH / 2) * nW) + i) ];
				mesh.addVertex( ofVec3f( ofMap(i, 0, nW, sideOnRect.x, sideOnRect.x + sideOnRect.width ), sideOnRect.y + (val * sideOnRect.height) ) );
			}
			mesh.draw();
			
			if( drawGui ) { gui.draw(); }
		}
	
		// -----------------------------------------------------------------------------------------
		float fbm( ofVec2f _loc, int _octaves, float _lacunarity, float _persistence, int _pixelStoreIndex = -1 )
		{
			float finalNoise = 0.0;
			float amplitude = 1.0;
			float totalAmplitude = 0.0;
			ofVec2f tmpLoc = _loc;
			
			for( int i = 0; i < _octaves; i++)
			{
				amplitude *= _persistence;
				totalAmplitude += amplitude;
				float layerNoise = ofNoise(tmpLoc.x, tmpLoc.y);
				finalNoise += layerNoise * amplitude;
				tmpLoc *= _lacunarity; // //sum += amp * snoise(pp);
				
				// terrible hack, this should not be here
				if( _pixelStoreIndex != -1 ) { layers.at(i).getPixelsRef()[_pixelStoreIndex] = layerNoise; }
				if( _pixelStoreIndex ==  0 ) { layerAmplitude[i] = amplitude; }
			}
			
			return finalNoise / totalAmplitude;
		}
	
		// -----------------------------------------------------------------------------------------
		void keyPressed( int _key )
		{
			if( _key == ' ' )			  { }
			else if( _key == OF_KEY_TAB ) { drawGui = !drawGui; }
			else if( _key == 'f' )		  { ofToggleFullscreen(); }
		}

		// -----------------------------------------------------------------------------------------
		void smoothStepCurve( ofVec2f _start, ofVec2f _end, int _res = 30 )
		{
			ofMesh tmpMesh;
			tmpMesh.setMode( OF_PRIMITIVE_LINE_STRIP );
			for( int k = 0; k < _res; k++ )
			{
				ofVec3f tmpPoint = _start.getInterpolated( _end, k / (float)(_res-1) );
				tmpPoint.y = ofMap( smoothstep(_start.x, _end.x, tmpPoint.x), 0, 1, _start.y, _end.y );
				tmpMesh.addVertex( tmpPoint );
			}
			tmpMesh.draw();
		}
	
		// -----------------------------------------------------------------------------------------
		float smoothstep(float edge0, float edge1, float x)
		{
			x = ofClamp( (x - edge0)/(edge1 - edge0), 0, 1); // Scale, and clamp x to 0..1 range
			return x*x*x*(x*(x*6 - 15) + 10); // Evaluate polynomial
		}
		
		// -----------------------------------------------------------------------------------------
		float smoothStepInOut( float _t, float _low0, float _high0, float _high1, float _low1 )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			float localLow1  = _low1;
			float localHigh1 = _high1;
			return smoothstep( localLow0, localHigh0, _t ) * (1.0f - smoothstep( localHigh1, localLow1, _t ));
		}
	
		int noiseStartX;
		int noiseStartY;
	
		vector<ofFloatImage> layers; // stores the result of each noise layer computation
		vector<float> layerAmplitude;
		ofFloatImage noiseResult;
	
		ofxPanel gui;
		
		ofParameter<int> numOctaves;
		ofParameter<float> frequency;
		ofParameter<float> lacunarity;
		ofParameter<float> persistence;
		
		bool drawGui;
	
		ofTrueTypeFont fontSmall;
};
