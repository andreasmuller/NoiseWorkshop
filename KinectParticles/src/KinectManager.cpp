
#include "KinectManager.h"

// --------------------------------------------------------------------------------
//
KinectManager::KinectManager()
{
	haveOpenedKinect = false;

	//meshNeedsUpdating = false;
	imagesNeedUpdating = false;
	//sensorThresholdNeedsUpdating = false;
}

// --------------------------------------------------------------------------------
//
void KinectManager::init()
{
	
	camera.setAutoDistance( false );
	camera.setPosition( ofVec3f(5154.15, -157.545, 1553.4) );
	camera.setOrientation( ofQuaternion(-0.718446, -0.00589216, -0.695435, 0.0083604) );

	camera.setFarClip( 4096 * 4 );
	

	// Set up UI

	// float param callbacks
	trackerMinAreaRadius.addListener(this, &KinectManager::trackerFloatParamChanged);
	trackerMaxAreaRadius.addListener(this, &KinectManager::trackerFloatParamChanged);
	trackerMaximumDistance.addListener(this, &KinectManager::trackerFloatParamChanged);

	// Int param callbacks
	trackerPersistence.addListener(this, &KinectManager::trackerIntParamChanged);
	
	// ofVec3f callbacks
	interactionAreaBoxPos.addListener(this, &KinectManager::trackerVec3ParamChanged);
	interactionAreaBoxSize.addListener(this, &KinectManager::trackerVec3ParamChanged);
	
	// bool callbacks
	blobTrackingEnabled.addListener(this, &KinectManager::trackerBoolParamChanged);

	string xmlSettingsPath = "Settings/KinectTracker.xml";
	gui.setup( "Kinect Tracker", xmlSettingsPath );
	
	//gui.add( kinectNearThreshold.set("Sensor Near Threshold mm",		400.0f, KINECT_SENSOR_NEAR_LIMIT, KINECT_SENSOR_FAR_LIMIT ) );
	gui.add( kinectFarThreshold.set( "Sensor Far Threshold mm",		   2500.0f, KINECT_SENSOR_NEAR_LIMIT, KINECT_SENSOR_FAR_LIMIT ) );

	gui.add( pointCloudStep.set( "Point Cloud Step",					3, 1, 10 ) );

	gui.add( blobTrackingEnabled.set( "Blob Tracking Enabled",			false ) );
	
	gui.add( trackerMinAreaRadius.set( "Tracker Min Area Radius",		4.0f, 1.0f, 300.0f ) );
	gui.add( trackerMaxAreaRadius.set( "Tracker Max Area Radius",		500.0f, 1.0f, 2000.0f ) );

	gui.add( trackerPersistence.set( "Tracker Persistence",				15, 1, 60 ) );

	// an object can move up to this many pixels per frame
	gui.add( trackerMaximumDistance.set( "Tracker Maximum Distance",		50.0f, 1.0f, 200.0f ) );

	gui.add( isSettingUpInteractionArea.set( "Set up Interaction Area",	false ) );

	gui.add( mask2DIsEnabled.set( "Mask 2D Enabled",	false ) );
	
	gui.add( interactionAreaBoxPos.set( "Interaction Area Box Pos",		ofVec3f(   0,   0,  1500 ),  ofVec3f( -2000,-2000,   0 ), ofVec3f( 2000, 2000, 4000 ) ) );
	gui.add( interactionAreaBoxSize.set( "Interaction Area Box Size",	ofVec3f( 500, 200, 1500 ),   ofVec3f(   100,  100, 100 ), ofVec3f( 4000, 4000, 4000 ) ) );

	gui.add( doOpticalFlow.set( "Do Optical Flow",	true ) );
	
	gui.loadFromFile( xmlSettingsPath );	

	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	
	string xmlFlowSettingsPath = "Settings/OpticalFlow.xml";
	guiFlow.setup( "Optical Flow", xmlFlowSettingsPath );
	
	guiFlow.add( flowPyrScale.set(		"Scale",		0.5, 0, 1) );
	guiFlow.add( flowLevels.set(		"Levels",		4, 1, 8) );
	guiFlow.add( flowWinSize.set(		"Win Size",		8, 4, 64) );
	guiFlow.add( flowIterations.set(	"Iterations",	2, 1, 8) );
	guiFlow.add( flowPolyN.set(			"Poly N",		7, 5, 10) );
	guiFlow.add( flowPolySigma.set(		"Poly Sigma",	1.5, 1.1, 2) );
	guiFlow.add( flowUseGaussian.set(	"Use Gaussian", true) );
	
	guiFlow.loadFromFile( xmlFlowSettingsPath );
	
	flowPyrScale.addListener(this, &KinectManager::flowFloatParamChanged );
	flowLevels.addListener(this, &KinectManager::flowIntParamChanged );
	flowWinSize.addListener(this, &KinectManager::flowIntParamChanged );
	flowIterations.addListener(this, &KinectManager::flowIntParamChanged );
	flowPolyN.addListener(this, &KinectManager::flowIntParamChanged );
	flowPolySigma.addListener(this, &KinectManager::flowFloatParamChanged );
	flowUseGaussian.addListener(this, &KinectManager::flowBoolParamChanged );
	
	updateFlowParams();
	
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
	
	bool infrared	= false;
	bool video		= true;
	bool useTexture = false;
	kinect.init( infrared, video, useTexture );
	
	
	haveOpenedKinect = kinect.open();		// opens first available kinect
	//haveOpenedKinect = kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//haveOpenedKinect = kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	kinect.setDepthClipping( KINECT_SENSOR_NEAR_LIMIT, KINECT_SENSOR_FAR_LIMIT );
	ofVec3f tmp;
	trackerVec3ParamChanged( tmp );

	// Contour Finder settings
	contourFinder.setMinAreaRadius(trackerMinAreaRadius);
	contourFinder.setMaxAreaRadius(trackerMaxAreaRadius);

	contourFinder.setAutoThreshold( false );
	contourFinder.setThreshold(15);

	// wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(trackerPersistence);
	// an object can move up to X pixels per frame
	contourFinder.getTracker().setMaximumDistance(trackerMaximumDistance);

	ofFile tmpMaskFile( "KinectTrackerMask/KinectTrackerMask.png" );
	ofImage tmpMaskImage;
	
	if( tmpMaskFile.exists() )
	{
		tmpMaskImage.loadImage( tmpMaskFile );
	}
		
	// If we have a mask, prepare the data for use
	if( tmpMaskImage.isAllocated() )
	{
		// if it's an RGB image we need to convert it to greyscale
		if( tmpMaskImage.getPixelsRef().getNumChannels() == 3 )
		{
			depthImageMask.allocate( tmpMaskImage.getWidth(), tmpMaskImage.getHeight(), OF_IMAGE_GRAYSCALE );
			unsigned char* dstPixels = depthImageMask.getPixels();
			unsigned char* srcPixels = tmpMaskImage.getPixels();

			int srcW = tmpMaskImage.getWidth();
			int dstIndex = 0;
			for( int y = 0; y < tmpMaskImage.getHeight(); y++ )
			{
				for( int x = 0; x < tmpMaskImage.getWidth(); x++ )
				{
					dstPixels[dstIndex] = srcPixels[ ((y*srcW) + x) * 3 ];
					dstIndex++;
				}
			}

			depthImageMask.update();
		}
		else if ( tmpMaskImage.getPixelsRef().getNumChannels() == 1 )
		{
			depthImageMask.clone( tmpMaskImage );
		}

		//depthImageMask.loadImage("KinectTrackerMask/KinectTrackerMask.png");
		if( depthImageMask.isAllocated() )
		{
			depthImageMaskMat = toCv(depthImageMask);
		}
	}
	
	opticalFlowPixelsFullSize.allocate( kinect.getWidth(), kinect.getHeight(), OF_IMAGE_COLOR );
	opticalFlowPixelsFullSizeMat = toCv( opticalFlowPixelsFullSize );

	opticalFlowPixelsSmallSize.allocate( kinect.getWidth() / 3, kinect.getHeight() / 3, OF_IMAGE_COLOR );
	opticalFlowPixelsSmallSizeMat = toCv( opticalFlowPixelsSmallSize );

	// Start the thread
	start();
}

// --------------------------------------------------------------------------------
//
void KinectManager::trackerFloatParamChanged(float & _newParam )
{
	contourFinder.setMinAreaRadius(trackerMinAreaRadius);
	contourFinder.setMaxAreaRadius(trackerMaxAreaRadius);

	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(trackerMaximumDistance);
}

// --------------------------------------------------------------------------------
//
void KinectManager::trackerIntParamChanged(int & _newParam )
{
	contourFinder.getTracker().setPersistence(trackerPersistence);
}

// --------------------------------------------------------------------------------
//
void KinectManager::trackerVec3ParamChanged( ofVec3f & _newParam )
{
	updateKinectClippingFromInteractionBox();
}

// --------------------------------------------------------------------------------
//
void KinectManager::trackerBoolParamChanged( bool & _newParam )
{
	lock();
		trackedObjectPos.clear();
		trackedObjectColor.clear();
	unlock();
}

// --------------------------------------------------------------------------------
//
void KinectManager::updateKinectClippingFromInteractionBox()
{
	float nearClip = interactionAreaBoxPos.get().z + (interactionAreaBoxSize.get().z * -0.5);
	float farClip  = interactionAreaBoxPos.get().z + (interactionAreaBoxSize.get().z *  0.5);
	
	nearClip = ofClamp( nearClip, 0, KINECT_SENSOR_FAR_LIMIT );
	
	if( interactionAreaBoxSize.get().length() > 0 )
	{
		kinect.setDepthClipping( nearClip, farClip );
		
		//cout << "- - - - - - - -" << endl;
		//cout << "interactionAreaBoxPos: " << interactionAreaBoxPos << " interactionAreaBoxSize: " << interactionAreaBoxSize << endl;
		//cout << "nearClip: " << nearClip << " farClip: " << farClip << endl;
		//cout << "- - - - - - - -" << endl;
	}
}

// --------------------------------------------------------------------------------
//
void KinectManager::flowFloatParamChanged(float & _newParam )
{
	updateFlowParams();
}

// --------------------------------------------------------------------------------
//
void KinectManager::flowIntParamChanged(int & _newParam )
{
	updateFlowParams();
}

// --------------------------------------------------------------------------------
//
void KinectManager::flowBoolParamChanged( bool & _newParam )
{
	updateFlowParams();
}

// --------------------------------------------------------------------------------
//
void KinectManager::updateFlowParams()
{
	denseFlow.setPyramidScale( flowPyrScale );
	denseFlow.setNumLevels( flowLevels);
	denseFlow.setWindowSize( flowWinSize );
	denseFlow.setNumIterations( flowIterations );
	denseFlow.setPolyN( flowPolyN );
	denseFlow.setPolySigma( flowPolySigma );
	denseFlow.setUseGaussian( flowUseGaussian );
}

// --------------------------------------------------------------------------------
//
void KinectManager::start()
{
	startThread();
}

// --------------------------------------------------------------------------------
//
void KinectManager::stop()
{
	waitForThread( true );
	kinect.close();
}

// --------------------------------------------------------------------------------
//
void KinectManager::threadedFunction()
{
	while(isThreadRunning())
	{
		kinect.update();	

		// set up the interaction box parameters
		interactionAreaBoundingBox.setPosition( interactionAreaBoxPos );
		interactionAreaBoundingBox.setSize( interactionAreaBoxSize );

		// there is a new frame and we are connected
		if( kinect.isFrameNew() && haveOpenedKinect )  
		{	
			lock();

				colorPixels.setFromPixels( kinect.getPixelsRef().getPixels(), kinect.getWidth(), kinect.getHeight(), 3 );
				colorPixelsMat = toCv( colorPixels );
			
				depthPixelsChar.setFromPixels( kinect.getDepthPixels(), kinect.getWidth(), kinect.getHeight(), 1 );
				depthPixelsCharMat = toCv( depthPixelsChar ); // shallow copy

				if( !maskedDepthPixels.isAllocated() )	
				{
					//maskedDepthPixels.allocate( depthPixelsChar.getWidth(), depthPixelsChar.getHeight(), OF_IMAGE_GRAYSCALE );
					maskedDepthPixels.setFromPixels( kinect.getDepthPixels(), kinect.getWidth(), kinect.getHeight(), 1 );
					maskedDepthPixelsMat = toCv(maskedDepthPixels);
				}

				bool maskIsEnabled = true;

				// clear first
				maskedDepthPixelsMat = cv::Scalar(0);

				if( depthImageMask.isAllocated() && mask2DIsEnabled )
				{
					depthPixelsCharMat.copyTo( maskedDepthPixelsMat, depthImageMaskMat );
				}
				else
				{
					depthPixelsCharMat.copyTo( maskedDepthPixelsMat );
				}
			
				// Grab a copy of our color image, masked by the depth image
				imagesNeedUpdating = true;

			unlock();

			// We unlock here before updating the pixels, might this be why we get flashes sometimes?
			
			if( !isSettingUpInteractionArea )
			{
				if( blobTrackingEnabled )
				{
					updateTracking(); // Confusingly this also updates the valid pixels point cloud
				}
				{
					updateValidPixelsPointCloud();
				}
				
				if( doOpticalFlow ) updateOpticalFlow();
			}
			else
			{
				updateInteractionAreaSetup();
			}
		}

		ofSleepMillis(1);
	}
}

// --------------------------------------------------------------------------------
//
void KinectManager::updateTracking()
{
	// I'm locking here as we draw the images later, reorganize this

	lock();
	
		// map our threshold from sensor space to a 255..0 range
		float threshGreyscaleSpace = ofMap( kinectFarThreshold,  KINECT_SENSOR_NEAR_LIMIT, KINECT_SENSOR_FAR_LIMIT, 255, 0 );
	
		contourFinder.findContours( maskedDepthPixelsMat );

		RectTracker& tracker = contourFinder.getTracker();

		// Convert all the active pixels to world space points to have a look at them
		activeWorldPointsMesh.clear();
		activeWorldPointsMesh.setMode( OF_PRIMITIVE_POINTS );

		trackedObjectPos.clear();
		trackedObjectColor.clear();

		vector<ofPolyline> polylines = contourFinder.getPolylines();
		for(int i = 0; i < (int)polylines.size(); i++) 
		{
			unsigned int label = contourFinder.getLabel(i);
			ofSeedRandom(label << 24);
			ofColor contourIdCol = ofColor::fromHsb(ofRandom(255), 255, 255);
			ofRectangle tmpBoundingRect = toOf(contourFinder.getBoundingRect(i));

			int numValidPixels = 0;
			ofVec3f averagePos(0,0,0);

			// Todo: use the tracker rather than the straight blobs

			// Todo: track the tip closest to the screen
			//		One approach could be to find the bounding box, then select the foremost 30% and do the calculation on that

			int step = pointCloudStep;
			for( int x = tmpBoundingRect.x; x < tmpBoundingRect.x + tmpBoundingRect.width; x += step )
			{
				for( int y = tmpBoundingRect.y; y < tmpBoundingRect.y + tmpBoundingRect.height; y += step )
				{
					int tmpIndex = (y * KINECT_RES_WIDTH) + x;

					if( depthPixelsChar[tmpIndex] > 0 )
					{
						ofVec3f tmpWorldPos =  kinect.getWorldCoordinateAt(x,y);

						if( interactionAreaBoundingBox.pointIsInside( tmpWorldPos ) )
						{
							averagePos += tmpWorldPos;

							activeWorldPointsMesh.addVertex( tmpWorldPos );
							//activeWorldPointsMesh.addColor(  colorPixels.getColor(x,y).getLerped( contourIdCol, 0.3f ) );
							activeWorldPointsMesh.addColor(  colorPixels.getColor(x,y) );

							numValidPixels++;
						}
					}
				}
			}

			if( numValidPixels > 0 )
			{
				averagePos /= numValidPixels;

				trackedObjectPos.push_back( averagePos );
				trackedObjectColor.push_back( contourIdCol );
			}
		}

		ofSeedRandom();

		doesHaveNewInteractionPoints = true; // or we have none now, but this is valid too
	
	unlock();
}


// --------------------------------------------------------------------------------
//
void KinectManager::updateOpticalFlow()
{
	opticalFlowPixelsFullSizeMat.setTo( 0 );
	colorPixelsMat.copyTo( opticalFlowPixelsFullSizeMat, depthPixelsCharMat );

	cv::resize( opticalFlowPixelsFullSizeMat, opticalFlowPixelsSmallSizeMat, opticalFlowPixelsSmallSizeMat.size(), 0, 0, INTER_LINEAR );
	
	denseFlow.calcOpticalFlow( opticalFlowPixelsSmallSizeMat );
}

// --------------------------------------------------------------------------------
//
void KinectManager::getCurrentPointCloud( vector<ofVec3f>& _points )
{
	_points.clear();
	
	lock();
	
		for( int i = 0; i < activeWorldPointsMesh.getNumVertices(); i++ )
		{
			_points.push_back( activeWorldPointsMesh.getVertex(i) );
		}
	
	unlock();
}

// --------------------------------------------------------------------------------
//
void KinectManager::getCurrentPointCloud( vector<ofVec3f>& _points, vector<ofFloatColor>& _colors )
{
	_points.clear();
	_colors.clear();
	
	lock();
	
		for( int i = 0; i < activeWorldPointsMesh.getNumVertices(); i++ )
		{
			_points.push_back( activeWorldPointsMesh.getVertex(i) );
			_colors.push_back( activeWorldPointsMesh.getColor(i) );
		}
	
	unlock();
}

// --------------------------------------------------------------------------------
//
void KinectManager::getCurrentPointCloudColors( vector<ofFloatColor>& _colors )
{
	_colors.clear();
	
	lock();
	
		for( int i = 0; i < activeWorldPointsMesh.getNumColors(); i++ )
		{
			_colors.push_back( activeWorldPointsMesh.getColor(i) );
		}
	
	unlock();
}

// --------------------------------------------------------------------------------
//
void KinectManager::updateValidPixelsPointCloud()
{
	// I'm locking here as we draw the images later, reorganize this
	
	lock();
	
	// Convert all the active pixels to world space points to have a look at them
	activeWorldPointsMesh.clear();
	activeWorldPointsMesh.setMode( OF_PRIMITIVE_POINTS );

	int step = pointCloudStep;
	for( int x = 0; x < KINECT_RES_WIDTH; x += step )
	{
		for( int y = 0; y < KINECT_RES_HEIGHT; y += step )
		{
			int tmpIndex = (y * KINECT_RES_WIDTH) + x;
			
			if( depthPixelsChar[tmpIndex] > 0 )
			{
				ofVec3f tmpWorldPos =  kinect.getWorldCoordinateAt(x,y);
				
				if( interactionAreaBoundingBox.pointIsInside( tmpWorldPos ) )
				{
					activeWorldPointsMesh.addVertex( tmpWorldPos );
					activeWorldPointsMesh.addColor(  colorPixels.getColor(x,y) );
				}
			}
		}
	}

	unlock();
}

// --------------------------------------------------------------------------------
//
void KinectManager::updateInteractionAreaSetup()
{
	// I'm locking here as we draw the images later, reorganize this

	lock();

		trackedObjectPos.clear();
		trackedObjectColor.clear();

		activeWorldPointsMesh.clear();
		activeWorldPointsMesh.setMode( OF_PRIMITIVE_POINTS );

		int step = pointCloudStep;
		for( int x = 0; x < KINECT_RES_WIDTH; x += step )
		{
			for( int y = 0; y < KINECT_RES_HEIGHT; y += step )
			{
				int tmpIndex = (y * KINECT_RES_WIDTH) + x;

				ofVec3f tmpWorldPos =  kinect.getWorldCoordinateAt(x,y);
				ofColor tmpColor = colorPixels.getColor(x,y);

				if( !interactionAreaBoundingBox.pointIsInside( tmpWorldPos ) )
				{
					tmpColor.a = 10;
				}
	
				activeWorldPointsMesh.addVertex( tmpWorldPos );
				activeWorldPointsMesh.addColor( tmpColor );
			}
		}

	unlock();
}

// --------------------------------------------------------------------------------
//
vector<ofVec2f> KinectManager::getInteractionPoints( ofRectangle _targetRect )
{
	vector< ofVec2f > returnPoints;

	lock();

		ofVec3f bbMin = interactionAreaBoundingBox.getPosition() + ( interactionAreaBoundingBox.getSize() * -0.5f );
		ofVec3f bbMax = interactionAreaBoundingBox.getPosition() + ( interactionAreaBoundingBox.getSize() *  0.5f );

		for( unsigned int i = 0; i < trackedObjectPos.size(); i++ )
		{
			ofVec3f tmpPosInBox = trackedObjectPos.at(i);
			ofVec2f tmp2DPos(0,0);

			tmp2DPos.x = ofMap( tmpPosInBox.x, bbMin.x, bbMax.x, _targetRect.x, _targetRect.x + _targetRect.width ); 
			tmp2DPos.y = ofMap( tmpPosInBox.z, bbMin.z, bbMax.z, _targetRect.y, _targetRect.y + _targetRect.height ); 

			returnPoints.push_back( tmp2DPos );
		}

	unlock();

	doesHaveNewInteractionPoints = false;

	return returnPoints;
}

// --------------------------------------------------------------------------------
//
void KinectManager::drawPointCloud()
{
	lock();

		ofEnableDepthTest();
	
		camera.begin();

			ofSetColor( ofColor::red );
			ofLine( ofVec3f(0,0,0), ofVec3f(0,0,KINECT_SENSOR_NEAR_LIMIT) );
			ofSetColor( ofColor::blue );
			ofLine( ofVec3f(0,0,KINECT_SENSOR_NEAR_LIMIT), ofVec3f(0,0,KINECT_SENSOR_FAR_LIMIT) );

			// let's draw some ticks at the meter
			for( int i = 0; i < 4; i++ )
			{
				ofDrawSphere( ofVec3f(0,0,(i+1)*1000), 10 );
			}
	
			ofSetColor( ofColor::white );
			glPointSize( 3.0f );
			activeWorldPointsMesh.draw();
			glPointSize( 1.0f );
	
			for( int i = 0; i < trackedObjectPos.size(); i++ )
			{
				ofSetColor( trackedObjectColor.at(i) );
				ofDrawSphere(  trackedObjectPos.at(i), 20.0f );
			}

			ofSetColor( ofColor::blue, 80 );
			interactionAreaBoundingBox.draw();

		camera.end();

	unlock();

	ofSetColor( ofColor::white );
}

// --------------------------------------------------------------------------------
//
void KinectManager::debugDraw2D()
{
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );

	if( imagesNeedUpdating )
	{
		lock();

			colorImage.setFromPixels( colorPixels );
			depthImageForContours.setFromPixels( maskedDepthPixels );
			maskedColorImageSmallOpticalFlow.setFromPixels( opticalFlowPixelsFullSize );

		unlock();

		imagesNeedUpdating = false;
	}

	if( colorImage.isAllocated() )
	{
		lock();

			ofPushMatrix();
		
				float tmpScale = 0.35;
				ofScale( tmpScale, tmpScale, tmpScale );

				ofSetColor( ofColor::white );
				depthImageForContours.setUseTexture( true ); // TODO: this is no good
				depthImageForContours.draw(10, 10, 640, 480);

				ofSetColor( ofColor::blueSteel, 90 );
				if( depthImageMask.isAllocated() && mask2DIsEnabled ) depthImageMask.draw(10, 10, 640, 480);

				if(mask2DIsEnabled)
				{
					ofSetColor( ofColor::white );
				}
				
				ofPushMatrix();

					ofTranslate(10,10);
					ofPushStyle();
					ofNoFill();
					vector<ofPolyline> polylines = contourFinder.getPolylines();
					for(int i = 0; i < (int)polylines.size(); i++) 
					{
						unsigned int label = contourFinder.getLabel(i);
						ofSeedRandom(label << 24);
						ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
						ofRect(toOf(contourFinder.getBoundingRect(i)));
					}
					ofPopStyle();

					ofSetColor( ofColor::white );
					ofSeedRandom();

				ofPopMatrix();

				ofSetColor( ofColor::white );
				colorImage.draw(640 + (10 * 2), 10, 640, 480);
		
				if( doOpticalFlow )
				{
					maskedColorImageSmallOpticalFlow.draw( (640 * 2) + (10 * 2), 10, 640, 480);
					drawFlow( ofRectangle((640 * 2) + (10 * 2), 10, 640, 480), true );
				}
		
			ofPopMatrix();

		unlock();
	}
}

// --------------------------------------------------------------------------------
//
void KinectManager::drawGui()
{
	gui.draw();
	
	if( doOpticalFlow )
	{
		guiFlow.setPosition( ofVec2f( gui.getPosition().x, gui.getPosition().y + gui.getHeight() + 10 ) );
		guiFlow.draw();
	}
}

// --------------------------------------------------------------------------------
//
void KinectManager::drawFlow( ofRectangle _rect, bool _showAverageFlow )
{
	ofVec2f offset(_rect.x,_rect.y);
	ofVec2f scale(_rect.width / denseFlow.getWidth(), _rect.height / denseFlow.getHeight());
	
	int stepSize = 4;
	float flowLineScale = 2.0f;
	
	ofMesh mesh;
	mesh.setMode( OF_PRIMITIVE_LINES );
	for(int y = 0; y < denseFlow.getHeight(); y += stepSize)
	{
		for(int x = 0; x < denseFlow.getWidth(); x += stepSize)
		{
			ofVec2f cur = ofVec2f(x, y) * scale + offset;
			mesh.addVertex( cur );
			mesh.addVertex( cur + (denseFlow.getFlowOffset(x, y) * scale * flowLineScale)  );
		}
	}
	ofSetColor( ofColor::white, 180 );
	mesh.draw();

	if( _showAverageFlow )
	{
		ofVec2f middle = _rect.position + (ofVec2f(_rect.width,_rect.height) * 0.5);
		ofVec2f averageFlow = denseFlow.getAverageFlow(); // We could do this by region, by blobs we are tracking.
		
		averageFlow *= 100;
		
		//cout << averageFlow << endl;
		ofSetColor( ofColor::red, 180 );
		ofSetLineWidth( 3 );
		ofLine( middle, middle + averageFlow );
		ofSetLineWidth( 1 );
	}
}

