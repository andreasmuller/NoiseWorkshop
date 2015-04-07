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
	
	string xmlSettingsPath = "Settings/Birds.xml";
	gui.setup( "Birds", xmlSettingsPath );
	
	gui.add( placementResolution.set("Resolution",  60,  3,  100) );
	gui.add( placementSize.set("Placement Size",  2,  0.1,  10) );
	gui.add( placementNoiseSpaceFrequency.set("Placement Space Freq",   1.0f,  0.0001f,  3.0f) );
	gui.add( placementBottomThreshold.set("Placement Bottom Threshold",  0,  0,  1) );
	
	gui.loadFromFile( xmlSettingsPath );
	
	// Register callbacks so that we regenerate the mesh when needed
	placementResolution.addListener( this, &ofApp::intParamChanged );
	placementSize.addListener( this, &ofApp::floatParamChanged );
	placementNoiseSpaceFrequency.addListener( this, &ofApp::floatParamChanged );
	placementBottomThreshold.addListener( this, &ofApp::floatParamChanged );
	
	// Give us a starting point for the camera, we're changing the scale a bit here to keep things more intuitive
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 5, 15.0 );
	camera.setMovementMaxSpeed( 0.4f );

	// Make a wing mesh;
	float size = 1.0f;
	wingMesh.setMode( OF_PRIMITIVE_TRIANGLES );
	wingMesh.addNormal(ofVec3f(0,1,0) );
	wingMesh.addVertex( ofVec3f(0,0,0) );
	
	wingMesh.addNormal(ofVec3f(0,1,0) );
	wingMesh.addVertex( ofVec3f(0,0,-size) );
	
	wingMesh.addNormal(ofVec3f(0,1,0) );
	wingMesh.addVertex( ofVec3f(size * 0.55,0,size * -0.05) );
	
	
	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::intParamChanged(int& _param )
{

}

//--------------------------------------------------------------
void ofApp::floatParamChanged(float& _param )
{

}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{

}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	float mx = ofNormalize( ofGetMouseX(), 0, ofGetWidth() );
	float my = ofNormalize( ofGetMouseY(), 0, ofGetHeight() );
	
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
	
	ofEnableDepthTest();
	
	ofDisableLighting();
	
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(60) );
			ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 10, 12, false );
		ofPopMatrix();
	
		ofSetColor( ofColor::white );
	
		ofLight light;
		light.setPointLight();
		light.setPosition( ofVec3f( 5, 10, 2 ) );
		light.enable();
	
//		drawBirds();
	
		ofDisableLighting();
	
		//ofSetColor( light.getDiffuseColor() );
		//ofDrawSphere( light.getPosition(), 0.3 );
	
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	ofRectangle targetRect( (ofGetWidth() * 0.5) - 100, (ofGetHeight() * 0.5) - 100, 200, 200 );
	
	float time = ofGetElapsedTimef() * ofLerp(0.1, 3.0, mx);
	
	for( int i = 0; i < 10; i++ )
	{
		ofSeedRandom( i << 24 );
		
		ofVec2f pos;
		
		float t = (time * ofRandom( 0.9, 1.1 )) + ofRandom(10);
		
		pos.x = ofNoise( i, t * ofRandom( 0.9, 1.1 ) * ofLerp( 0.5, 1.0, ofNoise(t*0.1))) * targetRect.width;
		pos.y = ofNoise( t * ofRandom( 0.9, 1.1 ) * ofLerp( 0.5, 1.0, ofNoise(-t*0.12))) * targetRect.height;
		pos += targetRect.position;
	

		ofSetColor( ofFloatColor::fromHsb( ofRandom(1), 1, 1) );
		ofCircle( pos, 3 );
	}
	
	ofSetColor( ofColor::white );
	
	ofNoFill();
		ofRect( targetRect );
	ofFill();
	
	if( drawGui )
	{
		gui.draw();
	}
	
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}


//-----------------------------------------------------------------------------------------
//
void ofApp::drawBirds()
{
	float time = ofGetElapsedTimef() * 0.1;
	
	float halfW = 10;
	float randomPosOffsetMag = halfW * 0.001;
	
	int tmpIndex = 0;
	for( int y = 0; y < placementResolution; y++ )
	{
		float posY  = ofMap(y, 0, placementResolution-1, -halfW, halfW);
		float fracY = ofMap(y, 0, placementResolution, 0, 1);
		
		for( int x = 0; x < placementResolution; x++ )
		{
			//  Seeding our random number generator like this let's us consistently get the same random numbers for each bird
			ofSeedRandom(tmpIndex << 24);
			
			ofVec3f randomOffset( ofRandom(-randomPosOffsetMag,randomPosOffsetMag), ofRandom(-randomPosOffsetMag,randomPosOffsetMag), ofRandom(-randomPosOffsetMag,randomPosOffsetMag) );
			ofColor color = ofColor::fromHsb( ofRandom(255), 200, 200 );
			
			float posX  = ofMap(x, 0, placementResolution-1, -halfW, halfW);
			float fracX = ofMap(x, 0, placementResolution, 0, 1);
			
			ofVec3f pos = ofVec3f(posX, 0, posY);
			ofVec3f perlinPos = pos * placementNoiseSpaceFrequency;
			
			float posPerlinVal = ofNoise( perlinPos.x, perlinPos.z );
			
			float placementPerlinVal = ofMap( posPerlinVal, placementBottomThreshold, 1,	0, 1, true );
			
			if( placementPerlinVal > 0 )
			{
				float flapTime = (time * 140) + tmpIndex;
				float lookAheadTime = 0.05; // the further ahead we look, the smoother the rotation will be.
				
				//ofDrawSphere( pos, 0.2 );
				
				float movementNoiseSpaceFrequency = placementNoiseSpaceFrequency;
				ofVec3f movementPerlinPos = (pos + randomOffset) * movementNoiseSpaceFrequency;
				float movementPerlinTime = time + ofRandom( 100 );
				
				ofVec3f currentBirdPos  = ofVec3f(0,halfW*0.5,0) + getBirdPosNoise( movementPerlinPos, movementPerlinTime				  ) * halfW;
				ofVec3f nextBirdPos		= ofVec3f(0,halfW*0.5,0) + getBirdPosNoise( movementPerlinPos, movementPerlinTime + lookAheadTime ) * halfW;
				float heightDiff = currentBirdPos.y - nextBirdPos.y;
				
				float flapMagnitude  = ofMap( heightDiff, 0, 0.6, ofRandom(54, 60), 2, true ); // Degrees! Flap more when we are ascending, glide when we are descending
				float flapExtraAngle = ofMap( heightDiff, 0, 0.6, 0,				 15, true ); // Pull back the wings a bit when we descend
				
				ofPushMatrix();
				
					ofTranslate( currentBirdPos );

					ofMatrix4x4 tmpMat;
					tmpMat.makeLookAtMatrix( ofVec3f(0,0,0), (nextBirdPos - currentBirdPos).getNormalized(), ofVec3f(0,1,0) );

					// HACK, the object dissapears if I just multiply by the matrix received
					ofVec3f translation; ofQuaternion rotation; ofVec3f scale; ofQuaternion scaleOrientation;
					tmpMat.decompose( translation, rotation, scale, scaleOrientation );
					rotation.get( tmpMat ); // copy a rotation matrix into tmpMat
				
					ofMultMatrix( tmpMat );
				
					ofSetColor( color );
					drawBird( flapTime, flapMagnitude, flapExtraAngle );
				
					//ofLine( currentBirdPos, nextBirdPos );
				
				ofPopMatrix();
			}
			tmpIndex++;
		}
	}

	/*
	ofVec3f upVec = ofVec3f(0, 1, 0);
	
	vector<ofVec3f> points;

	ofMesh lineMesh; lineMesh.setMode( OF_PRIMITIVE_LINE_STRIP );

	float radius = 1.0;
	float angleInc = TWO_PI / (float)points.size();
	// use the upVec from the splinePt as the starting point
	ofVec3f forwardVec;
	ofVec3f crossVec = upVec; // per splinePt;
	crossVec.normalize();
	crossVec *= radius;
	for (int i = 0; i < points.size(); i++)
	{
		ofPoint pt = points[i];
		// at the location to the rotated vector //
		ofVec3f ringPointTransformed = crossVec + points[i];
		lineMesh.addVertex(ringPointTransformed);
		// incrementally rotate around the forward vector //
		crossVec.rotate(angleInc, forwardVec);
	}
	 */
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawBird( float _flapTime, float _flapMagnitude, float _extraFlapAngle )
{
	float flapAng = sinf(_flapTime) * _flapMagnitude;
	
	ofPushMatrix();
		ofRotateZ( flapAng + _extraFlapAngle );
		wingMesh.draw();
	ofPopMatrix();
	
	ofPushMatrix();
		ofScale(-1, 1, 1);
		ofRotateZ( flapAng + _extraFlapAngle );
		wingMesh.draw();
	ofPopMatrix();
}

//-----------------------------------------------------------------------------------------
//
ofVec3f ofApp::getBirdPosNoise( ofVec3f _noisePos, float _time )
{
	ofVec3f p(0,0,0);
	
	p.x += ofSignedNoise( _noisePos.x, _noisePos.y, _noisePos.z, _time );
	p.y += ofSignedNoise( _noisePos.y, _noisePos.z, _noisePos.x, _time );
	p.z += ofSignedNoise( _noisePos.z, _noisePos.x, _noisePos.y, _time );
	
	p.x += ofSignedNoise( _time, _noisePos.x, _noisePos.y, _noisePos.z );
	p.y += ofSignedNoise( _noisePos.x, _time, _noisePos.z, _noisePos.y );
	p.z += ofSignedNoise( _noisePos.z, _noisePos.y, _time, _noisePos.x );
	
	p *= 0.5;
	
	// TEST
//	p.x = sinf( ofMap( p.x, -1, 1, 0, TWO_PI ) );
//	p.z = cosf( ofMap( p.z, -1, 1, 0, TWO_PI ) );
	
	return p;
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
