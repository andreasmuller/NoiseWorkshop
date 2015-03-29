
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
	
	gui.add( kinectPointCloudScale.set( "Kinect Point Cloud Scale", 0.05, 0.0, 1.0 ) );
	gui.add( kinectPointCloudOffset.set( "Kinect Point Cloud Offset", ofVec3f(0,0,0), ofVec3f(-500,-500,-500), ofVec3f(500,500,500) ) );
	
	gui.add( globalAmbient.set("Global Ambient", ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Position.set( "Light 1 Position", ofVec3f(-5,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );
	gui.add( light1Diffuse.set("Light 1 Diffuse",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Ambient.set("Light 1 Ambient",   ofColor(50,50,50), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( light1Specular.set("Light 1 Specular", ofColor(255,255,255), ofColor(0,0,0,0), ofColor(255,255,255,255)) );

	gui.add( drawPointCloud.set( "Draw Point Cloud", false ) );

	//gui.add( testPos.set( "Test Pos", ofVec3f(-5,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );

	gui.loadFromFile( xmlSettingsPath );
	gui.minimizeAll();
	
	kinectManager.init();
	
	particles.init(); // number of particles is (texSize * texSize)
	
	ofVec2f guiPos = gui.getPosition();
	particles.gui.setPosition( guiPos + ofVec2f( gui.getWidth() + 10, 0) );
	
	// Give us a starting point for the camera
	camera.setNearClip( 0.01f );
	camera.setPosition( 0, 50, 100.0 );
	camera.setMovementMaxSpeed( 1.0f );
	
	pointCloudMesh.setMode( OF_PRIMITIVE_POINTS );
	
	time = 0.0f;
	timeStep = 1.0f / 60.0f;
	
	light[0].enable();

	opticalFlowDebugDrawer.init();
	opticalFlowDebugDrawer.getGui()->setPosition( gui.getPosition() + ofVec2f( 0, gui.getHeight() + 10) );
	
	ofSetLogLevel(OF_LOG_NOTICE);
	
	drawUI = false;
	
	//ofHideCursor();
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::update()
{
	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );} else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += timeStep;
	
	float mx = ofMap( ofGetMouseX(), 0, ofGetWidth()-1,  0, 1 );
	float my = ofMap( ofGetMouseY(), 0, ofGetHeight()-1, 0, 1 );
	
	ofSetGlobalAmbientColor( globalAmbient.get() );

	light[0].setAmbientColor( light1Ambient.get() ); // If you're having trouble passing an 'ofParameter<ClassX>' into something that expects a 'ClassX' use .get()
	light[0].setDiffuseColor( light1Diffuse.get() );
	light[0].setSpecularColor( light1Specular.get() );

	light[0].setPosition( light1Position );
	

	if( currAppMode == APP_MODE_LIVE  )
	{
		kinectToWorld.makeIdentityMatrix();
		kinectToWorld.scale( ofVec3f(kinectPointCloudScale) );
		kinectToWorld.scale( ofVec3f(1,-1,-1) );
		kinectToWorld.translate( kinectPointCloudOffset );
		
		worldToKinect = kinectToWorld.getInverse();
		
		particles.setWorldToFlowParameters( worldToKinect );
		
		
		// Copy the points and colors every frame for now, can be optimized
		kinectManager.getCurrentPointCloud( pointCloudPoints, pointCloudColors );
		
		if( pointCloudPoints.size() != pointCloudColors.size() ) ofLogError() << "ofApp::update() pointCloudPoints.size() != pointCloudColors.size()";
		
		
		//ofVec3f kinectCoordinateCorrection(1,-1,1);
		
		pointCloudMesh.clear();
		for( int i = 0; i < pointCloudPoints.size(); i++ )
		{
			//pointCloudMesh.addVertex( ((pointCloudPoints.at(i) * kinectPointCloudScale) * kinectCoordinateCorrection) + kinectPointCloudOffset );
			pointCloudMesh.addVertex( pointCloudPoints.at(i) * kinectToWorld );
			pointCloudMesh.addColor( pointCloudColors.at(i) );
		}
		
		// Spawn positions
		bool haveNewData = pointCloudMesh.getNumVertices() > 2; // We'll be grabbing the vertices from the mesh, a bit hacky but the rescaled data is right there
		if( haveNewData )
		{
			// Fill in the span buffer for the particles with random point cloud locations
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
		
		// Optical Flow
		
		ofxCv::FlowFarneback& flowObj = kinectManager.getFlowObject();
		int targetNumChannels = 3;
		
		if( particles.opticalFlowBuffer.getWidth() != flowObj.getWidth() ||
		   particles.opticalFlowBuffer.getHeight() != flowObj.getHeight() )
		{
			particles.opticalFlowBuffer.allocate( flowObj.getWidth(), flowObj.getHeight(), targetNumChannels );
			
			particles.opticalFlowTexture.allocate( flowObj.getWidth(), flowObj.getHeight(), GL_RGB32F, false );
			
			cout << "** Alocating flow " << particles.opticalFlowBuffer.getWidth() << ", " << particles.opticalFlowBuffer.getHeight() << "  " << particles.opticalFlowBuffer.getNumChannels() << endl;
		}
		
		// We are going to copy the flow data into am RGB texture rather than RG, having trouble getting that working,
		// otherwise it would (without filtering) just be ofxCv::toOf( flowObj.getFlow(), particles.opticalFlowBuffer );
		
		int numChannels = particles.opticalFlowBuffer.getNumChannels();
		
		int w = particles.opticalFlowBuffer.getWidth();
		int h = particles.opticalFlowBuffer.getHeight();
		int tmpIndex = 0;
		int windowSize = 4;
		int windowSizeHalf = windowSize / 2;
		for( int y = windowSize; y < h - windowSize; y++ )
		{
			for( int x = windowSize; x < w - windowSize; x++ )
			{
				tmpIndex = ((y*w) + x) * numChannels;
				//ofVec2f flowOffset = flowObj.getFlowOffset( x, y );
				ofVec2f flowOffset = flowObj.getAverageFlowInRegion( ofRectangle( x-windowSizeHalf, y-windowSizeHalf, windowSize, windowSize) );
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 0 ] = flowOffset.x;
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 1 ] = flowOffset.y;
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 2 ] = 0;
			}
		}
		
		particles.opticalFlowTexture.loadData( particles.opticalFlowBuffer );
		
		particles.setAverageFlow( kinectManager.getAverageFlowSmoothed() );
		
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
			opticalFlowDebugDrawer.getGui()->draw();
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
	
	string tmpStr = ofToString( kinectManager.getNumFramesProcessedPerSecond(), 1) + " " + ofToString( ofGetFrameRate(), 1 );
	fontSmall.drawStringShadowed( tmpStr, ofVec2f(ofGetWidth()-60, ofGetHeight()-10) );
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::drawScene()
{
	ofEnableDepthTest();
	
	camera.begin();
	
		if( drawUI )
		{
			// draw a grid on the floor
			ofSetColor( ofColor(10) );
			ofPushMatrix();
				ofRotate(90, 0, 0, -1);
				ofDrawGridPlane( 100, 10, false );
			ofPopMatrix();
		}
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
		particles.draw( &camera );
	
		ofDisableLighting();

		if( opticalFlowDebugDrawer.isEnabled() ) opticalFlowDebugDrawer.draw(  particles.opticalFlowTexture, worldToKinect, particles.getFlowMaxLength() );
	
		//ofSetColor( ofColor::blue );
		//ofDrawSphere( testPos, 1.0 );
	
		//ofSetColor( ofColor::red );
		//ofDrawSphere( testPos * worldToKinect, 10.0 );
	
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