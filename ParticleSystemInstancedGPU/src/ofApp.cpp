#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetDefaultWidth( 300 );

	int texSize = 128;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
	// Give us a starting point for the camera
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 0.5, 1.1 );
	camera.setMovementMaxSpeed( 0.01f );

	time = 0.0f;
	timeStep = 1.0f / 60.0f;

	
	// UI for the light and material
	string xmlSettingsPathLight = "Settings/LightAndMaterial.xml";
	guiLight.setup( "Light And Material", xmlSettingsPathLight );
	guiLight.add( globalAmbient.set("Global Ambient", ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	guiLight.add( light1Position.set("Light 1 Position",   ofVec3f( -0.2, 0.35, 0.0 ), ofVec3f( -1, 0, -1), ofVec3f( 1, 2, 1)) );
	guiLight.add( light1Diffuse.set("Light 1 Diffuse",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	guiLight.add( light1Ambient.set("Light 1 Ambient",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	guiLight.add( light1Specular.set("Light 1 Specular", ofColor(255,255,255), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	guiLight.loadFromFile( xmlSettingsPathLight );
	guiLight.minimizeAll();
	
	drawGui = false;
	
	//light[0].setGlobalPosition( ofVec3f( -0.2, 0.7, 0.1 ) );

	light[0].enable();
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );}
	else { timeStep = ofLerp( timeStep, 1.0f / 120.0f, 0.1f ); } //  *********************** TEMP, slowing down time a bit, set back to normal time once we change the sim
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
	
		ofEnableLighting();
	
			ofSetGlobalAmbientColor( globalAmbient );
			light[0].setGlobalPosition( light1Position );
			light[0].setAmbientColor( light1Ambient.get() ); // If you're having trouble passing an 'ofParameter<Class>' into something that expects a 'Class' use .get()
			light[0].setDiffuseColor( light1Diffuse.get() );
			light[0].setSpecularColor( light1Specular.get() );
		
			particles.draw( &camera );

		ofDisableLighting();
	
		if( drawGui )
		{
			ofSetColor( light[0].getDiffuseColor() );
			ofDrawSphere( light[0].getGlobalPosition(), 0.01 );
		}
		
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	int size = 196;
	//particles.particleDataFbo.source()->getTextureReference(0).draw( 0,	 0, size, size );
	
	if( drawGui )
	{
		guiLight.draw();
		particles.drawGui();
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
