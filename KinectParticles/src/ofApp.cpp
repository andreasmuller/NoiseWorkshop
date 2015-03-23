
#include "ofApp.h"

// ------------------------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	ofBackground( ofColor::white );
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	currAppMode = APP_MODE_LIVE;

	ofxGuiSetFont( "Fonts/DIN.otf", 8 );
	ofxGuiSetDefaultWidth( 260 );

	// Set up UI
	string xmlSettingsPath = "Settings/Main.xml";
	gui.setup( "Main", xmlSettingsPath );
	
	gui.add( kinectPointCloudScale.set( "Kinect Point Cloud Scale", 0.05, 0.0, 0.1 ) );
	gui.add( kinectPointCloudOffset.set( "Kinect Point Cloud Offset", ofVec3f(0,0,0), ofVec3f(-100,-100,-100), ofVec3f(100,100,100) ) );
	

	gui.loadFromFile( xmlSettingsPath );
	
	//kinectPointCloudScale = 0.05;
	kinectManager.init();
	
	int texSize = 64;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
	ofVec2f guiPos = gui.getPosition();
	particles.gui.setPosition( guiPos + ofVec2f( 0, gui.getHeight() + 10) );
	
	// Give us a starting point for the camera
	camera.setNearClip( 0.01f );
	camera.setPosition( 0, 50, 100.0 );
	camera.setMovementMaxSpeed( 1.0f );
	
	pointCloudMesh.setMode( OF_PRIMITIVE_POINTS );
	
	time = 0.0f;
	timeStep = 1.0f / 60.0f;
	
	ofSetLogLevel(OF_LOG_NOTICE);
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );} else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += timeStep;
	
	
	if( currAppMode == APP_MODE_LIVE  )
	{
		// Copy the points every frame for now, but can be optimized
		kinectManager.getCurrentPointCloud( pointCloudPoints, pointCloudColors );
		
		if( pointCloudPoints.size() != pointCloudColors.size() ) ofLogError() << "ofApp::update() pointCloudPoints.size() != pointCloudColors.size()";
		
		ofVec3f kinectCoordinateCorrection(1,-1,1);
		
		pointCloudMesh.clear();
		for( int i = 0; i < pointCloudPoints.size(); i++ )
		{
			pointCloudMesh.addVertex( ((pointCloudPoints.at(i) * kinectPointCloudScale) * kinectCoordinateCorrection) + kinectPointCloudOffset );
			pointCloudMesh.addColor( pointCloudColors.at(i) );
		}
		
		bool haveNewData = pointCloudMesh.getNumVertices() > 2; // We'll be grabbing the vertices from the mesh, a bit hacky but the data is right there
		if( haveNewData )
		{
			int tmpIndex = 0;
			for( int y = 0; y < particles.textureSize; y++ )
			{
				for( int x = 0; x < particles.textureSize; x++ )
				{
					int randomIndex = (int)ofRandom(pointCloudMesh.getNumVertices()-1);
					ofVec3f spawnPos = pointCloudMesh.getVertex( randomIndex );
					
					particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 0 ] = spawnPos.x;
					particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 1 ] = spawnPos.y;
					particles.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 2 ] = spawnPos.z;
					
					tmpIndex++;
				}
			}
			
			particles.spawnPosTexture.loadData( particles.spawnPosBuffer );
		}
	}
	else if ( currAppMode == APP_MODE_TOUCH_SETUP )
	{
		
	}
	
}


// ------------------------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
	
	if( currAppMode == APP_MODE_LIVE  )
	{
		particles.update( time, timeStep );
		
		drawScene();
		
		ofDisableDepthTest();
		
		if( drawUI )
		{
			kinectManager.debugDraw2D();
			gui.draw();
			particles.drawGui();
		}
	}
	else if ( currAppMode == APP_MODE_TOUCH_SETUP )
	{

		kinectManager.drawPointCloud();
		kinectManager.debugDraw2D();

		ofSetColor( ofColor::white );

		if( drawUI )
		{
			kinectManager.drawGui();
		}
	}
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::drawScene()
{
	ofEnableDepthTest();
	
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(10) );
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 30, 10, false );
		ofPopMatrix();
	
		//ofDrawAxis(100);

		glPointSize( 3.0 );
		pointCloudMesh.draw();
		glPointSize( 1.0 );
	
		ofSetColor( ofColor::white );
		particles.draw( &camera );
	
	camera.end();
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::exit() 
{
	kinectManager.stop();
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key){
	
	if( key == 'f')
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_TAB )
	{
		drawUI = !drawUI;
	}
	else if( key == OF_KEY_RIGHT )
	{
		currAppMode++;
		if( currAppMode >= APP_MODE_AMOUNT ) currAppMode = 0;
	}
	else if( key == OF_KEY_LEFT )
	{
		currAppMode--;
		if( currAppMode < 0 ) currAppMode = APP_MODE_AMOUNT - 1;		
	}
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::mouseMoved(int x, int y )
{
	lastTimeMouseMoved = ofGetElapsedTimef();

}


