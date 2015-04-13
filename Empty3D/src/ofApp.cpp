#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	// Give us a starting point for the camera
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 4, 10 );
	camera.setMovementMaxSpeed( 0.1f );
	
}

//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);	
	
	ofEnableDepthTest();
	
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(60) );
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 10, 10, false );
		ofPopMatrix();

	
	camera.end();

	ofSetColor( ofColor::white );
	ofDisableDepthTest();


	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == 'f' )
	{
		ofToggleFullscreen();
	}
}
