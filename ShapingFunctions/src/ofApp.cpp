#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate( 60 );
	time = 0;
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
	float w = ofGetWidth();
	float h = ofGetHeight();
	
	float screenLengthInSecs = 4.0f;
	
	float mx = ofMap( ofGetMouseX(), 0, ofGetWidth(),  0.0f, 1.0f );
	float my = ofMap( ofGetMouseY(), 0, ofGetHeight(), 0.0f, 1.0f );
	
	float mouseSpeed = 1.0; //ofMapUnit( mx, 0.1f, 3.0f );
	
	if( !ofGetKeyPressed(' ') )
	{
		time += ofGetLastFrameTime() * mouseSpeed;
	}
	
	ofBackgroundGradient( ofColor( 10, 10, 40), ofColor::black, OF_GRADIENT_CIRCULAR );
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	// Draw some lines to show where the unit edges are (0,1,2, etc)
	ofMesh bgSecondsMesh;
	bgSecondsMesh.setMode( OF_PRIMITIVE_LINES );
	for( int i = 0; i < (int)screenLengthInSecs; i++ )
	{
		float pixelsPerSec = w / screenLengthInSecs;
		float tmpX = w - ((fmodf( time, 1.0 ) + i) * pixelsPerSec);
		bgSecondsMesh.addVertex( ofVec2f( tmpX, 0) );
		bgSecondsMesh.addVertex( ofVec2f( tmpX, h) );
	}
	ofSetColor( ofColor::white, 100 );
	bgSecondsMesh.draw();
	
	
	float magnitude = 100;
	
	ofMesh mesh;
	mesh.setMode( OF_PRIMITIVE_LINE_STRIP );
	int steps = 300;
	for( int i = 0; i < steps; i++ )
	{
		float frac = i / (float)steps;
		float vertexTime = ofMap( i, 0, steps, time, time + screenLengthInSecs );
		
		ofVec2f pos;
		pos.x = ofMap( i,  0, steps,  0, w );
		pos.y = h * 0.5f;
		pos.y += getTerrain( vertexTime, magnitude );
		
		mesh.addVertex( pos );
	}
	ofSetColor( ofColor::white );
	mesh.draw();
	
	ofLine( ofVec2f( 0, ofGetHeight()/2 ), ofVec2f( ofGetWidth(), ofGetHeight()/2 ) );	// draw a line showing the middle

	// Todo: make everything scroll with the control keys, have the option to autoplay.
	ofSetColor( ofColor::slateGray, 20 );
	ofLine( 0, (h * 0.5f) - 100, ofGetWidth(), (h * 0.5f) - magnitude );
	ofLine( 0, (h * 0.5f) + 100, ofGetWidth(), (h * 0.5f) + magnitude );
	
}
//--------------------------------------------------------------
float ofApp::getTerrain( float _t, float _magnitude )
{
	float my = ofMap( ofGetMouseY(), 0, ofGetHeight(), 0.0f, 1.0f );
	
	float result = 0.0f;
	
	result += MathUtils::pulseTriangle( 3.0, 1.0, _t ) * _magnitude;
	//result += MathUtils::pulseLineDownUp( 3.0, 1.0, _t ) * _magnitude;
	//result += MathUtils::pulseLineUpDown( 3.0, 1.0, _t ) * _magnitude;
	//result += MathUtils::pulseSine( 3.0, 0.5, _t ) * _magnitude;
	//result += MathUtils::pulseSmoothStep( 3.0, 0.0, 0.3, 0.8, 1.4, _t ) * _magnitude;
	//result += MathUtils::pulseTriangle( 3.0, 1.5, _t ) * _magnitude;
	//result += MathUtils::pulseLinearStep( 3.0, 0.0, 0.3, 0.8, 1.4, _t ) * _magnitude;
	//result += MathUtils::pulseSawTooth( 3.0, 1.0, _t ) * _magnitude;
	//result += ofNoise( _t * 8.0 )  * MathUtils::pulseSmoothStep( 3.0, 0.0, 0.3, 0.8, 1.4, _t );
	//result += ofNoise( _t * 20.0 );// * MathUtils::pulseSmoothStep( 10.0, 0.0, 1.0, 3.0, 4.5, _t );
	//result += ofNoise( _t * 1.5 );
	
	result *= _magnitude;
	
	return result;
}

