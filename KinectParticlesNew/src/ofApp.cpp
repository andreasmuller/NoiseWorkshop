
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

	gui.add( testPos.set( "Test Pos", ofVec3f(-5,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );

	gui.add( debugFlow.set( "Debug Flow", true ) );
	gui.add( debugFlowRes.set( "Debug Flow Res", 20, 4, 100 ) );
	gui.add( debugFlowPos.set( "Debug Flow Pos", ofVec3f(0,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );
	gui.add( debugFlowSize.set( "Debug Flow Size", ofVec2f(100,100), ofVec2f(-600,-600), ofVec2f(600,600) ) );

	
	gui.loadFromFile( xmlSettingsPath );
	gui.minimizeAll();
	
	kinectManager.init();
	
	int texSize = 64;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
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

	ofSetLogLevel(OF_LOG_NOTICE);
	
	
	// We need to set a few extra params for the geometry shader, in this order.
	debugOpticalFlowShader.setGeometryInputType(GL_LINES);
	debugOpticalFlowShader.setGeometryOutputType(GL_LINE_STRIP );
	debugOpticalFlowShader.setGeometryOutputCount(12);
	debugOpticalFlowShader.load( "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.vert",
								 "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.frag",
								 "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.geom");
	
	
	drawUI = false;
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
		
		// -- TEMP -------------
		ofVec3f p = testPos * worldToKinect;
		
		double HFOV = 1.01447;	// Grabbed these by querying with OpenNI, so will probably only work with the Kinect V1
		double VFOV = 0.789809;
		
		double fXToZ = tan(HFOV/2)*2;
		double fYToZ = tan(VFOV/2)*2;
		
		double fCoeffX = 640 / fXToZ;
		double fCoeffY = 480 / fYToZ;
		
		int nHalfXres = 640 / 2;
		int nHalfYres = 480 / 2;
		
		testDepthImagePos.x = (float)fCoeffX * p.x / p.z + nHalfXres;
		testDepthImagePos.y = nHalfYres - (float)fCoeffY * p.y / p.z;
		testDepthImagePos.y = 480 - testDepthImagePos.y;
		
		kinectManager.testDepthMapPos = testDepthImagePos;
		// ----------------------
		

		
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
		// otherwise it would (without filtering) be ofxCv::toOf( flowObj.getFlow(), particles.opticalFlowBuffer );

		//Mat& flowData = flowObj.getFlow();
		int numChannels = particles.opticalFlowBuffer.getNumChannels();
		
		int w = particles.opticalFlowBuffer.getWidth();
		int h = particles.opticalFlowBuffer.getHeight();
		int tmpIndex = 0;
		//int windowSize = 4;
		for( int y = 0; y < h; y++ )
		{
			//float tmpY = ofMap( y, 0, h-1,	0, 1 );
			for( int x = 0; x < w; x++ )
			{
				//tmpIndex = ((y*w) + x) * numChannels;
				//float tmpX = ofMap( x, 0, w-1,	0, 1 );
				ofVec2f flowOffset = flowObj.getFlowOffset( x, y );
				//ofVec2f flowOffset = ofVec2f(x,y) - ofVec2f(w/2,h/2);
				//ofVec2f flowOffset = (ofVec2f(x,y) - ofVec2f(w/2,h/2)).getNormalized() * 1.0;
				//ofVec2f flowOffset = ofVec2f(ofMap( x, 0, w-1,	0, 1 ),ofMap( y, 0, h-1,	0, 1 ));
				//ofVec2f flowOffset = ofVec2f(0,ofMap( y, 0, h-1,	0, 1 ));
				//ofVec2f flowOffset = ofVec2f(ofMap( x, 0, w-1,	-1, 1 ),0);
				//ofVec2f flowOffset = flowObj.getAverageFlowInRegion( ofRectangle( x-1, y-1, 2, 2) ); // we can also grab an average in a window
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 0 ] = flowOffset.x;// * mx;
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 1 ] = flowOffset.y;// * mx;
				particles.opticalFlowBuffer.getPixels()[ tmpIndex + 2 ] = 0;
				//particles.opticalFlowBuffer.getPixels()[ tmpIndex + 3 ] = 0;
				
				tmpIndex += numChannels;
			}
		}
		
		particles.opticalFlowTexture.loadData( particles.opticalFlowBuffer );
		
		//		glTypeInternal;
		//		textureTarget;
		//if( ofGetFrameNum() % 20 == 0 ) ofLogNotice() << particles.opticalFlowTexture.getTextureData().tex_t << " " << particles.opticalFlowTexture.getTextureData().tex_u;
		//if( ofGetFrameNum() % 20 == 0 ) ofLogNotice() << particles.opticalFlowTexture.getTextureData().tex_w << " " << particles.opticalFlowTexture.getTextureData().tex_h;
		//if( ofGetFrameNum() % 20 == 0 ) ofLogNotice() << particles.opticalFlowTexture.getTextureData().glTypeInternal;// << " " << particles.opticalFlowTexture.getTextureData().textureTarget;
		//if( ofGetFrameNum() % 20 == 0 ) ofLogNotice() << particles.opticalFlowTexture.getTextureData().;
		
		//if( particles.opticalFlowTexture.getTextureData().textureTarget == GL_TEXTURE_RECTANGLE_ARB ) { ofLogNotice() << "GL_TEXTURE_RECTANGLE_ARB";}
		//else if( particles.opticalFlowTexture.getTextureData().textureTarget == GL_TEXTURE_2D )		  { ofLogNotice() << "GL_TEXTURE_2D";}
		
		//ofLogNotice() << ofGetImageTypeFromGLType( particles.opticalFlowTexture.getTextureData().glTypeInternal );
		//ofGetGlFormat(particles.opticalFlowBuffer)
		//ofGetGlType(particles.opticalFlowBuffer)
		
		//GL_RGB32F
		
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
	
	string tmpStr = ofToString( kinectManager.getNumFramesProcessedPerSecond(), 1) + " " + ofToString( ofGetFrameRate(), 1 );
	fontSmall.drawStringShadowed( tmpStr, ofVec2f(ofGetWidth()-60, ofGetHeight()-10) );
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
		particles.draw( &camera );
	
		ofDisableLighting();

		if( debugFlow ) debugDrawOpticalFlow();
	
		ofSetColor( ofColor::blue );
		ofDrawSphere( testPos, 1.0 );
	
		ofSetColor( ofColor::red );
		ofDrawSphere( testPos * worldToKinect, 10.0 );
	
	camera.end();
}

// ------------------------------------------------------------------------------------------------------
//
void ofApp::debugDrawOpticalFlow()
{
	ofMesh tmpMesh;
	tmpMesh.setMode( OF_PRIMITIVE_LINES ); // Expanding points in the geom shader isn't working, work around by using lines
	for( int y = 0; y < debugFlowRes; y++ )
	{
		for( int x = 0; x < debugFlowRes; x++ )
		{
			ofVec3f p = debugFlowPos.get() +  ofVec3f( ofMap( x, 0, debugFlowRes-1, debugFlowSize.get().x * -0.5, debugFlowSize.get().x * 0.5),
													   ofMap( y, 0, debugFlowRes-1, debugFlowSize.get().y * -0.5, debugFlowSize.get().y * 0.5),
													   0 );
			
			ofVec2f flowOffset = (ofVec2f(x,y) - ofVec2f(debugFlowRes/2,debugFlowRes/2)).getNormalized() * 2.0;
			
			tmpMesh.addVertex( p );
			tmpMesh.addVertex( p + flowOffset );
		}
	}
	

	debugOpticalFlowShader.begin();
	
	debugOpticalFlowShader.setUniformTexture( "u_opticalFlowMap", particles.opticalFlowTexture, 3 );
	
	debugOpticalFlowShader.setUniformMatrix4f("u_worldToKinect",  worldToKinect );
	
	double HFOV = 1.01447;	// Grabbed these by querying with OpenNI, so will probably only work with the Kinect V1
	double VFOV = 0.789809;
	
	debugOpticalFlowShader.setUniform1f("u_fXToZ", tan(HFOV/2)*2 );
	debugOpticalFlowShader.setUniform1f("u_fYToZ", tan(VFOV/2)*2 );

		ofSetColor( ofColor::white);
		tmpMesh.draw();

	debugOpticalFlowShader.end();

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