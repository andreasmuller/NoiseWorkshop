
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
		// -- TEMP -------------
		
		ofVec3f p = testPos * worldToKinect;
		
		/*
		double ref_pix_size = kinectManager.getKinectDevice()->getZeroPlanePixelSize();
		double ref_distance = kinectManager.getKinectDevice()->getZeroPlaneDistance();
		
		double factor = 2 * ref_pix_size * p.z / ref_distance;
		
		testDepthImagePos.x = (double)(p.x + 640/2) / factor;
		testDepthImagePos.y = (double)(p.y + 480/2) / factor;
		
		//cout << factor << endl;
		*/
		
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
		
		
		//cout << testDepthImagePos << endl;
		// ---------------------
		
		// fovRadians.fHFOV: 1.01447
		// fovRadians.fVFOV: 0.789809
		
		
		/*
			XnFieldOfView FOV;
		 xnGetDepthFieldOfView(m_hNode, &FOV);
		 
		 m_fRealWorldXtoZ = tan(FOV.fHFOV/2)*2;
		 m_fRealWorldYtoZ = tan(FOV.fVFOV/2)*2;
		 */
		
		/*
		 XN_C_API XnStatus xnConvertRealWorldToProjective(XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aRealWorld, XnPoint3D* aProjective)
		 {
			 XnStatus nRetVal = XN_STATUS_OK;
			 
			 XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
			 
			 // X_proj = X_res * (X_RW / (z*x_to_z) + 1/2)
			 //		= X_res / x_to_z * X_RW / z + X_res/2     (more efficient)
			 
			XnMapOutputMode outputMode;
			nRetVal = xnGetMapOutputMode(hInstance, &outputMode);
			XN_IS_STATUS_OK(nRetVal);
			
			xn::DepthPrivateData* pDepthPrivate = (xn::DepthPrivateData*)hInstance->pPrivateData;
			XnDouble fXToZ = pDepthPrivate->GetRealWorldXtoZ();
			XnDouble fYToZ = pDepthPrivate->GetRealWorldYtoZ();
			
			XnDouble fCoeffX = outputMode.nXRes / fXToZ;
			XnDouble fCoeffY = outputMode.nYRes / fYToZ;
			
			// we can assume resolution is even (so integer div is sufficient)
			XnUInt32 nHalfXres = outputMode.nXRes / 2;
			XnUInt32 nHalfYres = outputMode.nYRes / 2;
			
			for (XnUInt32 i = 0; i < nCount; ++i)
			{
				aProjective[i].X = (XnFloat)fCoeffX * aRealWorld[i].X / aRealWorld[i].Z + nHalfXres;
				aProjective[i].Y = nHalfYres - (XnFloat)fCoeffY * aRealWorld[i].Y / aRealWorld[i].Z;
				aProjective[i].Z = aRealWorld[i].Z;
			}
			
			return (XN_STATUS_OK);
		}
		 */
		
		/*
			XnFieldOfView FOV;
			xnGetDepthFieldOfView(m_hNode, &FOV);
			m_fRealWorldXtoZ = tan(FOV.fHFOV/2)*2;
			m_fRealWorldYtoZ = tan(FOV.fVFOV/2)*2;
		 */
		
		/*
		 /// camera -> world coordinate helper function
		 void freenect_camera_to_world(freenect_device* dev, int cx, int cy, int wz, double* wx, double* wy)
		 {
		 double ref_pix_size = dev->registration.zero_plane_info.reference_pixel_size;
		 double ref_distance = dev->registration.zero_plane_info.reference_distance;
		 
		 // We multiply cx and cy by these factors because they come from a 640x480 image,
		 // but the zero plane pixel size is for a 1280x1024 image.
		 // However, the 640x480 image is produced by cropping the 1280x1024 image
		 // to 1280x960 and then scaling by .5, so aspect ratio is maintained, and
		 // we should simply multiply by two in each dimension.
		 
		 double factor = 2 * ref_pix_size * wz / ref_distance;
		 
		 *wx = (double)(cx - DEPTH_X_RES/2) * factor;
		 *wy = (double)(cy - DEPTH_Y_RES/2) * factor;
		 }
		 */
		
		
		// Copy the points and colors every frame for now, can be optimized
		kinectManager.getCurrentPointCloud( pointCloudPoints, pointCloudColors );
		
		if( pointCloudPoints.size() != pointCloudColors.size() ) ofLogError() << "ofApp::update() pointCloudPoints.size() != pointCloudColors.size()";
		
		kinectToWorld.makeIdentityMatrix();
		kinectToWorld.scale( ofVec3f(kinectPointCloudScale) );
		kinectToWorld.scale( ofVec3f(1,-1,1) );
		kinectToWorld.translate( kinectPointCloudOffset );
		
		worldToKinect = kinectToWorld.getInverse();
		
		ofVec3f kinectCoordinateCorrection(1,-1,1);
		
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
		
		// Optical Flow
		
		ofxCv::FlowFarneback& flowObj = kinectManager.getFlowObject();
		int flowNumChannels = 2;
		
		if( particlesGeometry.opticalFlowBuffer.getWidth() != flowObj.getWidth() ||
		   particlesGeometry.opticalFlowBuffer.getHeight() != flowObj.getHeight() )
		{
			particlesGeometry.opticalFlowBuffer.allocate( flowObj.getWidth(), flowObj.getHeight(), flowNumChannels );
			
			cout << "** Alocating flow " << particlesGeometry.opticalFlowBuffer.getWidth() << ", " << particlesGeometry.opticalFlowBuffer.getHeight() << "  " << particlesGeometry.opticalFlowBuffer.getNumChannels() << endl;
		}
		
		ofxCv::toOf( flowObj.getFlow(), particlesGeometry.opticalFlowBuffer );
		particlesGeometry.opticalFlowTexture.loadData( particlesGeometry.opticalFlowBuffer );
		
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

		ofSetColor( ofColor::blue );
		ofDrawSphere( testPos, 1.0 );
	
	ofSetColor( ofColor::red );
	ofDrawSphere( testPos * worldToKinect, 10.0 );
	
	
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