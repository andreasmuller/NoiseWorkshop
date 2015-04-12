#pragma once

#include "ofMain.h"
#include "Math/MathUtils.h"

typedef MathUtils shape; // Shortcut to make the code a tad more redable later

class ofApp : public ofBaseApp
{
	public:
	
		// -----------------------------------------------------------------------
		void setup()
		{
			ofSetFrameRate( 60 );
			screenLengthInSecs = 4.0f;
			autoPlay = true;
			keyTimeVel = 0;
			time = 0;
		}

		// -----------------------------------------------------------------------
		void update()
		{
			
			if( ofGetKeyPressed( OF_KEY_LEFT ) )
			{
				keyTimeVel = ofLerp( keyTimeVel, screenLengthInSecs * -0.02f, 0.02 );
				autoPlay = false;
			}
			else if ( ofGetKeyPressed( OF_KEY_RIGHT ) )
			{
				keyTimeVel = ofLerp( keyTimeVel, screenLengthInSecs *  0.02, 0.02 );
				autoPlay = false;
			}
			
			if( autoPlay )
			{
				time += ofGetLastFrameTime();
			}
			else
			{
				time += keyTimeVel;
			}
			
			keyTimeVel = ofLerp( keyTimeVel, 0.0f, 0.05 );
		}
	
		// -----------------------------------------------------------------------
		void draw()
		{
			float w = ofGetWidth();
			float h = ofGetHeight();
			float middleY = h * 0.7;
			float magnitude = 300;
			ofRectangle terrainRect( 0, middleY-magnitude, w, magnitude );
			
			float mx = ofMap( ofGetMouseX(), 0, ofGetWidth(),  0.0f, 1.0f );
			float my = ofMap( ofGetMouseY(), 0, ofGetHeight(), 0.0f, 1.0f );
			
			ofBackgroundGradient( ofColor( 10, 10, 40), ofColor::black, OF_GRADIENT_CIRCULAR );
			ofEnableBlendMode( OF_BLENDMODE_ALPHA );
			
			ofSetColor( ofColor::white );
			ofLine( ofVec2f( 0, middleY), ofVec2f( ofGetWidth(), middleY ) );	// draw a line showing the middle

			bool drawClean = ofGetKeyPressed(' ');
			
			if( !drawClean )
			{
				// Draw some lines to show where the unit edges are (0,1,2, etc)
				drawDashes( time, (int)screenLengthInSecs, w / screenLengthInSecs );
				drawTerrain( terrainRect, ofGetWidth(), time, screenLengthInSecs );
			}
			
			// I'm storing this in a ofVec3f instead of a ofVec2f only because the oF ofVec2f doesn't have a cross product operator
			ofVec2f terrainMousePos 	= getTerrain( ofGetMouseX(), terrainRect, time, screenLengthInSecs );
			
			// We can easily get a normal for the point by simply sampling our terrain a little bit ahead in time and taking the cross product.
			// The further ahead we look, the smoother our normals will be, but look ahead too far and the normal isn't very representative
			float lookAheadPixels = 4;
			ofVec2f terrainMouseNormal = (terrainMousePos - getTerrain( ofGetMouseX() + lookAheadPixels, terrainRect, time, screenLengthInSecs)).getNormalized().getRotated(90);

			
			ofSetColor( ofColor::lightPink );
			ofCircle( terrainMousePos, 6.0 );
			
			if( !drawClean ) ofLine( terrainMousePos, terrainMousePos + (terrainMouseNormal*12));
			
			ofSetColor( ofColor::slateGray, 20 );
			ofNoFill();
				ofRect( terrainRect );
			ofFill();
		}
	
		// -----------------------------------------------------------------------
		float getTerrainHeight( float _t )
		{
			// We've set things up so the time units are seconds for this function
			float result = 0.0f;

			//result += shape::pulseSquare( 6.0, 1.0, _t );
			//result += shape::pulseTriangle( 3.0, 1.0, _t );
			//result += shape::pulseTriangle( 1.0, 1.0, _t );
			//result += shape::pulseLineDownUp( 3.0, 1.0, _t );
			//result += shape::pulseLineUpDown( 3.0, 1.0, _t );
			//result += shape::pulseSine( 3.0, 0.5, _t );
			//result += shape::pulseSmoothStep( 3.0, 0.0, 0.3, 0.8, 1.4, _t );
			//result += shape::pulseTriangle( 3.0, 1.5, _t );
			//result += shape::pulseLinearStep( 3.0, 0.0, 1.0, 2.0, 3.0, _t );
			//result += shape::pulseSmoothStep( 3.0, 0.0, 1.0, 1.2, 2.4, _t );
			//result += shape::pulseTriangle( 0.5, 0.5, _t ) * 0.2;
			//result += shape::pulseSine( 0.5, 0.5, _t * (ofMap(ofNoise( _t * 0.3 ), 0, 1, 0.2, 1.0)) ) * 0.2;
			//result += shape::pulseSawTooth( 3.0, 1.0, _t );
			//result += ofNoise( _t * (16.0 * ofNoise( _t * 0.1 )) );// * shape::pulseSmoothStep( 3.0, 0.0, 0.3, 0.8, 1.4, _t );
			//result += ofSignedNoise( _t * 1.0 ) * shape::pulseSmoothStep( 10.0, 0.0, 1.0, 3.0, 4.5, _t );
			//result *= ofNoise( _t * 0.1 );
			
			//result += ofNoise( _t * ((shape::pulseTriangle( 1.0, 1.0, _t ) * 2) + 2) );
			
			//result += ofNoise( _t * ofMap( shape::pulseTriangle( 10.0, 10.0, _t ), 0, 1, 0.01, 1) );
			result += shape::fbm( _t * 1.4, 5 );
			//result += ofNoise( _t * 2 );
			
			//result += (shape::pulseTriangle( 10.0, 10.0, _t ) * 0.8) + (ofNoise( _t * 2 ) * 0.2);
			
			//result *= ofNoise( _t * 0.03 );
			
			//result /= 2.0;
			
			return result;
		}
	
		// -----------------------------------------------------------------------
		ofVec2f getTerrain( float _x, ofRectangle _rect, float _time, float _rectWidthInSecs )
		{
			float vertexTime = ofMap( _x, 0, _rect.width, _time, _time + _rectWidthInSecs );
			
			ofVec2f pos;
			pos.x = _x;
			pos.y = -getTerrainHeight( vertexTime ) * _rect.height;
			pos += _rect.position + ofVec2f(0, _rect.height);
			
			return pos;
		}

		// -----------------------------------------------------------------------
		void drawTerrain( ofRectangle _rect, int _res, float _time, float _screenLengthInSecs  )
		{
			float w = ofGetWidth();
			
			ofMesh mesh;
			mesh.setMode( OF_PRIMITIVE_LINE_STRIP );

			for( int i = 0; i < _res; i++ )
			{
				float frac = i / (float)_res;
				float vertexTime = ofMap( i, 0, _res, _time, _time + _screenLengthInSecs );
				
				ofVec2f pos;
				pos.x = ofMap( i,  0, _res,  0, _rect.width );
				pos.y = -getTerrainHeight( vertexTime ) * _rect.height;
				
				pos += _rect.position + ofVec2f(0, _rect.height);// + pos;
				
				mesh.addVertex( pos );
			}
			ofSetColor( ofColor::white );
			mesh.draw();
		}

	
		// -----------------------------------------------------------------------
		void drawDashes( float _time, int _numDashes, float _pixelsPerSec )
		{
			float w = ofGetWidth();
			float h = ofGetHeight();
			ofMesh bgSecondsMesh;
			bgSecondsMesh.setMode( OF_PRIMITIVE_LINES );
			for( int i = 0; i < _numDashes; i++ )
			{
				float tmpX = w - ((fmodf( time, 1.0 ) + i) * _pixelsPerSec);
				bgSecondsMesh.addVertex( ofVec2f( tmpX, 0) );
				bgSecondsMesh.addVertex( ofVec2f( tmpX, h) );
			}
			ofSetColor( ofColor::white, 100 );
			bgSecondsMesh.draw();
		}
	
		// -----------------------------------------------------------------------
		void keyPressed( int _key )
		{
			if( _key == 'a' )
			{
				autoPlay = true;
			}
		}
	
		float time;
		float screenLengthInSecs;
		float keyTimeVel;
		bool autoPlay;
};
