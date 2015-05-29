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
	camera.setPosition( 0, 0.5, 0.9 );
	camera.setMovementMaxSpeed( 0.01f );

	time = 0.0f;
	timeStep = 1.0f / 60.0f;

	videoPlayer.loadMovie("Movies/SilhouetteTest.mov");
	videoPlayer.setLoopState( OF_LOOP_NORMAL );
	videoPlayer.play();
	
	drawGui = false;
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );}
	else { timeStep = ofLerp( timeStep, 1.0f / 120.0f, 0.1f ); } //  *********************** TEMP, slowing down time a bit, set back to normal time once we change the sim
	time += timeStep;
	
	bool haveNewData = false;
	
	videoPlayer.update();
	
	if( videoPlayer.isFrameNew() )
	{
		spawnPositions.clear();
	 
		ofPixels& maskPixels = videoPlayer.getPixelsRef();
		int channels = maskPixels.getNumChannels();
		int w = maskPixels.getWidth();
		int h = maskPixels.getHeight();
		float relwh = w/h;
		float relhw = h/w;
		
		int tmpIndex = 0;
		for( int y = 0; y < h; y++ )
		{
			for( int x = 0; x < w; x++ )
			{
				ofVec3f maskValue;
				maskValue.x = maskPixels[ (tmpIndex * channels) + 0 ];
				maskValue.y = maskPixels[ (tmpIndex * channels) + 1 ];
				maskValue.z = maskPixels[ (tmpIndex * channels) + 2 ];
				
				// Decide on whether to include the data at pixel or not
				if( (maskValue.x + maskValue.y + maskValue.z) > 0 )
				{
					// Turn our valid pixel into a 3D position, pretty arbitrary in this case
					// but let's fudge some numbers that work ok
					ofVec3f spawnPos;
					spawnPos.x = ofMap( x, 0, w, -0.5, 0.5 );
					spawnPos.y = ofMap( y, 0, h,  0.0, 0.75 );
					spawnPos.z = ofMap( maskValue.x, 0, 255.0, -0.2, 0.2 );
					
					spawnPositions.push_back( spawnPos );
				}
				
				tmpIndex++;
			}
		}
		
		haveNewData = true;
	}

	
/*
	spawnPositions.clear();
	float time = ofGetElapsedTimef();
	ofVec3f spawnMiddle = ofVec3f(0,0.2,0) + ofVec3f(0,0.2,0).getRotated( time * 30.0, ofVec3f(0,0,1) ).getRotated( time * 25.0, ofVec3f(0,1,0) );
	ofMesh sphereMesh = ofMesh::sphere( 0.08, 40 );
	//emissionMesh.clear();
	for( int y = 0; y < particles.textureSize; y++ )
	{
		for( int x = 0; x < particles.textureSize; x++ )
		{
			ofVec3f spawnPos = spawnMiddle + sphereMesh.getVertex( ofRandom( sphereMesh.getNumVertices() - 1) );
			spawnPositions.push_back( spawnPos );
		}
	}
	haveNewData = true;
 */
	

	if( haveNewData )
	{
		int tmpIndex = 0;
		for( int y = 0; y < particles.textureSize; y++ )
		{
			for( int x = 0; x < particles.textureSize; x++ )
			{
				ofVec3f spawnPos;
				
				spawnPos = spawnPositions.at( (int)ofRandom(spawnPositions.size()-1) );
				
				particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 0 ] = spawnPos.x;
				particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 1 ] = spawnPos.y;
				particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 2 ] = spawnPos.z;
				
				tmpIndex++;
			}
		}
		
		particles.spawnPosTexture.loadData( particles.spawnPosBuffer );
	}
	
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
			ofDrawGridPlane( 0.5, 12, false ); // of 0.9.0
		ofPopMatrix();
		 
		ofSetColor( ofColor::white );
		particles.draw( &camera );
	
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	int size = 196;
	//particles.particleDataFbo.source()->getTextureReference(0).draw( 0,	 0, size, size );
	videoPlayer.draw( 0,0, videoPlayer.getWidth() * 0.3, videoPlayer.getHeight() * 0.3 );
	
	if( drawGui )
	{
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
