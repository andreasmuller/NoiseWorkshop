#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetDefaultWidth( 300 );

	int texSize = 400;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
	// Give us a starting point for the camera
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 0.7, 1.5 );
	camera.setMovementMaxSpeed( 0.1f );

	time = 0.0f;
	timeStep = 1.0f / 60.0f;
		
	drawGui = false;
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );}
	else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += timeStep;
	
}




//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);	

	particles.update( time, timeStep );
	
	ofEnableDepthTest();
	
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(60) );
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 0.5, 10, false ); // of 0.9.0
		ofPopMatrix();
		 
		ofSetColor( ofColor::white );
		particles.draw( &camera );
	
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	int size = 196;
	//particles.particleDataFbo.source()->getTextureReference(0).draw( 0,	 0, size, size );
	
	if( drawGui )
	{
		particles.gui.draw();
	}
	
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == OF_KEY_TAB )
	{
		drawGui = !drawGui;
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_LEFT  )
	{
	}
	else if( key == OF_KEY_RIGHT  )
	{
	}
}
