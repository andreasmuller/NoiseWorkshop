
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
	
	gui.add( globalAmbient.set("Global Ambient", ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Position.set( "Light 1 Position", ofVec3f(-5,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );
	gui.add( light1Diffuse.set("Light 1 Diffuse",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Ambient.set("Light 1 Ambient",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Specular.set("Light 1 Specular", ofColor(255,255,255), ofColor(0,0,0,0), ofColor(255,255,255,255)) );

	gui.add( drawPointCloud.set( "Draw Point Cloud", false ) );

	gui.loadFromFile( xmlSettingsPath );

	kinectManager.init();
	
	int texSize = 128;
	particlesGeometry.init( texSize ); // number of particles is (texSize * texSize)
	
	ofVec2f guiPos = gui.getPosition();
	particlesGeometry.gui.setPosition( guiPos + ofVec2f( gui.getWidth() + 10, 0) );
	
	// Give us a starting point for the camera
	camera.setNearClip( 0.01f );
	camera.setPosition( 0, 50, 100.0 );
	camera.setMovementMaxSpeed( 1.0f );
	
	pointCloudMesh.setMode( OF_PRIMITIVE_POINTS );
	
	time = 0.0f;
	timeStep = 1.0f / 60.0f;
	
	light[0].enable();

	ofSetLogLevel(OF_LOG_NOTICE);
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );} else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += timeStep;
	
	ofSetGlobalAmbientColor( globalAmbient.get() );

	light[0].setAmbientColor( light1Ambient.get() ); // If you're having trouble passing an 'ofParameter<ClassX>' into something that expects a 'ClassX' use .get()
	light[0].setDiffuseColor( light1Diffuse.get() );
	light[0].setSpecularColor( light1Specular.get() );

	light[0].setPosition( light1Position );
	

	if( currAppMode == APP_MODE_LIVE  )
	{
		// Copy the points and colors every frame for now, can be optimized
		kinectManager.getCurrentPointCloud( pointCloudPoints, pointCloudColors );
		
		if( pointCloudPoints.size() != pointCloudColors.size() ) ofLogError() << "ofApp::update() pointCloudPoints.size() != pointCloudColors.size()";
		
		ofVec3f kinectCoordinateCorrection(1,-1,1);
		
		pointCloudMesh.clear();
		for( int i = 0; i < pointCloudPoints.size(); i++ )
		{
			pointCloudMesh.addVertex( ((pointCloudPoints.at(i) * kinectPointCloudScale) * kinectCoordinateCorrection) + kinectPointCloudOffset );
			pointCloudMesh.addColor( pointCloudColors.at(i) );
		}
		
		bool haveNewData = pointCloudMesh.getNumVertices() > 2; // We'll be grabbing the vertices from the mesh, a bit hacky but the rescaled data is right there
		if( haveNewData )
		{
			// Fill in the span buffer for the particles with random point cloud locations
			int tmpIndex = 0;
			for( int y = 0; y < particlesGeometry.textureSize; y++ )
			{
				for( int x = 0; x < particlesGeometry.textureSize; x++ )
				{
					int randomIndex = (int)ofRandom(pointCloudMesh.getNumVertices()-1);
					ofVec3f spawnPos = pointCloudMesh.getVertex( randomIndex );
					
					particlesGeometry.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 0 ] = spawnPos.x;
					particlesGeometry.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 1 ] = spawnPos.y;
					particlesGeometry.spawnPosBuffer.getPixels()[ (tmpIndex * 3) + 2 ] = spawnPos.z;
					
					tmpIndex++;
				}
			}
			
			particlesGeometry.spawnPosTexture.loadData( particlesGeometry.spawnPosBuffer );
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
		particlesGeometry.update( time, timeStep );
		
		drawScene();
		
		ofDisableDepthTest();
		
		if( drawUI )
		{
			kinectManager.debugDraw2D();
			gui.draw();
			particlesGeometry.drawGui();
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
			ofDrawGridPlane( 100, 10, false );
		ofPopMatrix();
	
		//ofDrawAxis( 100 );
	
		ofSetColor( light[0].getDiffuseColor() );
		ofDrawSphere( light[0].getGlobalPosition(), 0.2 );
	
		//ofDrawAxis(100);

		if( drawPointCloud )
		{
			glPointSize( 3.0 );
			pointCloudMesh.draw();
			glPointSize( 1.0 );
		}

		ofEnableLighting();
		light[0].enable();

		ofSetColor( ofColor::white );
		particlesGeometry.draw( &camera );
	
		ofDisableLighting();

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
	else if( key == 'p')
	{
		drawPointCloud = !drawPointCloud;
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