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

	string xmlSettingsPath = "Settings/Particles.xml";
	gui.setup( "Particles", xmlSettingsPath );
	
	gui.add( numParticles.set("Num particles",  300,  1,  10000) );
	gui.add( randomOffset.set("Random Offset",	0.1f, 0, 5) );
	
	gui.add( spaceFrequency.set("Space Frequency",   1.0f,  0.0001f,  1.0f) );
	gui.add( timeFrequency.set("Time Frequency",	1.0f,  0.0001f,  2.0f) );

	gui.add( noiseMagnitude.set("Noise Magnitude",	1.0f,  0.0001f,  2.0f) );
	
	gui.add( oldVelAmount.set("Old Vel Amount",	0.9f, 0, 1) );

	gui.add( maxVel.set("Max Vel",	0.1f, 0, 1) );
	gui.add( maxAge.set("Max Age",	3.0f, 0, 10) );

	gui.loadFromFile( xmlSettingsPath );
	
	// Register callbacks
	numParticles.addListener( this, &ofApp::numParticlesChanged );
	
	int tmpNumParticles = numParticles;
	numParticlesChanged( tmpNumParticles );
	
	// Give us a starting point for the camera, we're changing the scale a bit here to keep things more intuitive
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 5, 15.0 );
	camera.setMovementMaxSpeed( 0.4f );
	
	drawGui = false;
}

//-----------------------------------------------------------------------------------------
//
void ofApp::numParticlesChanged(int& _num )
{
	particles.resize( _num );
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		reInitParticle( &particles.at(i) );
		particles.at(i).age = ofRandom( maxAge );
	}
}

//-----------------------------------------------------------------------------------------
//
void ofApp::reInitParticle( Particle* _p )
{
	ofVec3f spawnSize(10,10,10);
	
	_p->pos = ofVec3f(  ofRandom( spawnSize.x * -0.5, spawnSize.x * 0.5),
						ofRandom( spawnSize.y *  0.0, spawnSize.y * 1.0),
						ofRandom( spawnSize.z * -0.5, spawnSize.z * 0.5) );
	
	_p->vel = ofVec3f(0);
	
	_p->noiseRandomOffset = ofVec3f( ofRandom( 0, randomOffset),
									 ofRandom( 0, randomOffset),
									 ofRandom( 0, randomOffset) );
	
	_p->age = 0;
	
	_p->posHistory.clear();
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	float time = ofGetElapsedTimef();
	float timeDelta = ofGetLastFrameTime();
	
	float noiseReadTime = time * timeFrequency;
	int tailLength = 6;
	
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		Particle* p = &particles.at(i);
		particles.at(i).age += timeDelta;
		
		if( particles.at(i).age >= maxAge )
		{
			reInitParticle( p );
		}
		
		ofVec3f noiseReadPos = (p->pos + p->noiseRandomOffset ) * spaceFrequency;
		p->vel = (getNoise( noiseReadPos, noiseReadTime ) * noiseMagnitude).getInterpolated( p->vel, oldVelAmount );
		p->vel.limit( maxVel );
		
		p->pos += p->vel;
		
		// Save positions to use as a tail
		p->posHistory.push_front( p->pos );
		while( p->posHistory.size() > tailLength ) { p->posHistory.pop_back(); }
	}
}

//-----------------------------------------------------------------------------------------
//
ofVec3f ofApp::getNoise( ofVec3f _noisePos, float _time )
{
	ofVec3f p(0,0,0);
	
//	p.x += ofSignedNoise( _noisePos.x, _noisePos.y, _noisePos.z, _time );
//	p.y += ofSignedNoise( _noisePos.y, _noisePos.z, _noisePos.x, _time );
//	p.z += ofSignedNoise( _noisePos.z, _noisePos.x, _noisePos.y, _time );
	
	p.x += ofSignedNoise( _noisePos.x, _noisePos.y, _noisePos.z, _time );
	p.y += ofSignedNoise( _noisePos.y, _noisePos.z, _time,		 _noisePos.x );
	p.z += ofSignedNoise( _noisePos.z, _time,		_noisePos.x, _noisePos.y );

	return p;
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
			ofDrawGridPlane( 10, 12, false );
		ofPopMatrix();
	
		ofSetColor( ofColor::white );
		drawParticles();
	
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
void ofApp::drawParticles()
{
	float time = ofGetElapsedTimef() * 0.1;
	
	ofMesh mesh;
	mesh.setMode( OF_PRIMITIVE_LINES );
	for( unsigned int i = 0; i < particles.size(); i++ )
	{
		Particle* p = &particles.at(i);
		
		ofSeedRandom( i << 24 );
		ofFloatColor tmpCol = ofFloatColor::fromHsb( ofRandom(1.0), 0.8, 0.8 );
		tmpCol.a = MathUtils::smoothStepInOut(0, maxAge * 0.1, maxAge * 0.9, maxAge, p->age);
		
		if( p->posHistory.size() > 1 )
		{
			for( int j = 1; j < p->posHistory.size(); j++ )
			{
				//mesh.addColor( tmpCol );
				mesh.addColor( ofFloatColor(tmpCol, ofMap( j-1, 0, p->posHistory.size()-1, 1, 0)) );
				mesh.addVertex( p->posHistory.at(j-1) );
				
				//mesh.addColor( tmpCol );
				mesh.addColor( ofFloatColor(tmpCol, ofMap( j  , 0, p->posHistory.size()-1, 1, 0)) );
				mesh.addVertex( p->posHistory.at(j) );
			}
		}
		//ofDrawSphere( particles.at(i).pos, 0.2 );
	}
	mesh.draw();
	
	ofSeedRandom();
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
