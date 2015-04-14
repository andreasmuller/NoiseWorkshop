#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetFont( "Fonts/DIN.otf", 8 );
	ofxGuiSetDefaultWidth( 300 );
	
	string xmlSettingsPath = "Settings/Grass.xml";
	gui.setup( "Grass", xmlSettingsPath );
	
	gui.add( placementResolution.set("Resolution",  60,  3,  400) );
	gui.add( placementSize.set("Placement Size",  2,  0.1,  10) );
	gui.add( placementNoiseSpaceFrequency.set("Placement Space Freq",   1.0f,  0.0001f,  3.0f) );
	gui.add( placementBottomThreshold.set("Placement Bottom Threshold",  0,  0,  1) );
	
	
	gui.add( stalkWidth.set("Stalk Width",  0.1f,  0.0001f,  1.0f) );
	gui.add( stalkHeight.set("Stalk Height",  1.0f,  0.0001f,  10.0f) );
	gui.add( swayingMaxAngle.set("Swaying Max Angle",  20.0,  0.0f,  180.0f) );
	gui.add( swayingTimeScale.set("Swaying Time Scale",  1.0f,  0.0001f,  3.0f) );
	gui.add( swayingNoiseSpaceFrequency.set("Swaying Noise Space Freq",  0.1f,  0.0001f,  5.0f) );
	gui.add( swayingTimeMaxDifference.set("Swaying Time Max Diff",  1.0f,  0.0001f,  5.0f) );
	
	gui.loadFromFile( xmlSettingsPath );
	
	// Register callbacks so that we regenerate the mesh when needed
	placementResolution.addListener( this, &ofApp::intParamChanged );
	placementSize.addListener( this, &ofApp::floatParamChanged );
	placementNoiseSpaceFrequency.addListener( this, &ofApp::floatParamChanged );
	placementBottomThreshold.addListener( this, &ofApp::floatParamChanged );
	
	// Give us a starting point for the camera, we're changing the scale a bit here to keep things more intuitive
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 0.5, 1.1 );
	camera.setMovementMaxSpeed( 0.1f );

	// We need to set a few extra params for the geometry shader, in this order.
	grassShader.setGeometryInputType(GL_LINES);
	grassShader.setGeometryOutputType(GL_TRIANGLE_STRIP );
	grassShader.setGeometryOutputCount(14);
	grassShader.load("Shaders/Grass/GL2/Grass.vert", "Shaders/Grass/GL2/Grass.frag", "Shaders/Grass/GL2/Grass.geom");
	
	computeMesh();
	
	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::intParamChanged(int& _param )
{
	computeMesh();
}

//--------------------------------------------------------------
void ofApp::floatParamChanged(float& _param )
{
	computeMesh();
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{

}

//-----------------------------------------------------------------------------------------
//
void ofApp::computeMesh()
{
	int resX = placementResolution;
	int resY = placementResolution;
	
	ofVec3f size( placementSize, 1, placementSize );
	ofVec3f placeStart = size * -0.5f;
	ofVec3f placeEnd   = size *  0.5f;
	
	grassMesh.clear();
	grassMesh.setMode( OF_PRIMITIVE_LINES );
	
	for( int y = 0; y < placementResolution; y++ )
	{
		float fracY = ofMap( y,  0, resY,  0, 1);
		for( int x = 0; x < resX; x++ )
		{
			float fracX = ofMap( x,  0, resX,  0, 1);
			
			ofVec3f tmpPos(0,0,0);
			tmpPos.x = ofLerp( placeStart.x, placeEnd.x, fracX );
			tmpPos.z = ofLerp( placeStart.z, placeEnd.z, fracY );
		
			ofVec2f perlinPos = ofVec2f(tmpPos.x, tmpPos.z) * placementNoiseSpaceFrequency; //ofMap( mx, 0, 1,  0.01, 5);
			
			float perlinVal = ofNoise( perlinPos.x, perlinPos.y );
			
			perlinVal = ofMap( perlinVal, placementBottomThreshold, 1,	0, 1, true ); // Remap the number taking our bottom threshold into account
			
			if( perlinVal > 0 )
			{
				grassMesh.addVertex( tmpPos );
				grassMesh.addVertex( tmpPos + ofVec3f(0,perlinVal,0) );
			}
		}
	}
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
			ofDrawGridPlane( 1, 21, false ); // of 0.9.0
		ofPopMatrix();
		 
		ofSetColor( ofColor::white );

		drawGrass( &camera );
	
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	if( drawGui )
	{
		gui.draw();
	}
	
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawGrass( ofCamera* _camera )
{
	float mx = ofMap( ofGetMouseX(), 0, ofGetWidth(),   0, 1 );
	float my = ofMap( ofGetMouseY(), 0, ofGetHeight(),  0, 1 );
	
	grassShader.begin();

		grassShader.setUniform1f("timeSecs", ofGetElapsedTimef() );

		grassShader.setUniform3fv( "cameraWorldPos", _camera->getGlobalPosition().getPtr() );

		grassShader.setUniform1f("stalkHalfWidth", stalkWidth / 2.0f );
		grassShader.setUniform1f("stalkHeight", stalkHeight );

		grassShader.setUniform1f("animationTimeMaxDifference", swayingTimeMaxDifference );

		grassShader.setUniform1f("stalkSwayingMaxAngle", ofDegToRad(swayingMaxAngle) * 0.5f );
	
		grassShader.setUniform1f("grassSwayingTimeScale", swayingTimeScale );
		grassShader.setUniform1f("grassSwayingNoiseFrequency", swayingNoiseSpaceFrequency );

		ofFloatColor grassColor;
		grassColor.setHsb( 	ofGetElapsedTimef() * 0.01, 0.87, 1.0f );
		ofSetColor( grassColor );
		//ofSetColor( ofColor::white );
	
		grassMesh.draw();
	
	grassShader.end();
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
