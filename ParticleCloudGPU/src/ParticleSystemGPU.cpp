//
//  ParticleSystemGPU.cpp
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#include "ParticleSystemGPU.h"

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::init( int _texSize )
{
	string xmlSettingsPath = "Settings/Main.xml";
	gui.setup( "Main", xmlSettingsPath );
	gui.add( particleMaxAge.set("Particle Max Age", 10.0f, 0.0f, 20.0f) );
	gui.add( noiseMagnitude.set("Noise Magnitude", 0.075, 0.01f, 2.0f) );
	gui.add( noisePositionScale.set("Noise Position Scale", 1.5f, 0.01f, 10.0f) );
	gui.add( noiseTimeScale.set("Noise Time Scale", 1.0 / 4000.0, 0.001f, 1.0f) );
	gui.add( noisePersistence.set("Noise Persistence", 0.2, 0.001f, 1.0f) );
	gui.add( baseSpeed.set("Wind", ofVec3f(0.5,0,0), ofVec3f(-2,-2,-2), ofVec3f(2,2,2)) );
	gui.add( startColor.set("Start Color", ofColor::white, ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( endColor.set("End Color", ofColor(0,0,0,0), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( particleSize.set("Particle Size", 0.01, 0.0001f, 0.05f) );
	//gui.add( twistNoiseTimeScale.set("Twist Noise Time Scale", 0.01, 0.0f, 0.5f) );
	//gui.add( twistNoisePosScale.set("Twist Noise Pos Scale", 0.25, 0.0f, 2.0f) );
	//gui.add( twistMinAng.set("Twist Min Ang", -1, -5, 5) );
	//gui.add( twistMaxAng.set("Twist Max Ang", 2.5, -5, 5) );
	
	gui.loadFromFile( xmlSettingsPath );
	

	// To use a texture with point sprites it will need to be created as a GL_TEXTURE_2D and not a GL_TEXTURE_RECTANGLE
	// This way it has texture coordinates in the range 0..1 instead of 0..imagewith
	ofDisableArbTex();
	particleImage.loadImage( "Textures/Soft64.png" );
	ofEnableArbTex();

	// Load shaders
#ifdef TARGET_OPENGLES
	particleUpdate.load("Shaders/Particles/GLES/Update");
	particleDrawUnsorted.load("Shaders/Particles/GLES/DrawUnsorted");
#else
	particleUpdate.load("Shaders/Particles/GL2/Update");
	particleDrawUnsorted.load("Shaders/Particles/GL2/DrawUnsorted");
#endif


	// Set how many particles we are going to have, this is based on data texture size
	textureSize = 400;
	numParticles = textureSize * textureSize;

	// Allocate buffers
	ofFbo::Settings fboSettings;
	fboSettings.width  = textureSize;
	fboSettings.height = textureSize;
	
	// We can create several color buffers for one FBO if we want to store velocity for instance,
	// then draw to them simultaneously from a shader using gl_FragData[0], gl_FragData[1], etc.
	fboSettings.numColorbuffers = 1;
	
	fboSettings.useDepth = false;
	fboSettings.internalformat = GL_RGBA32F;	// Gotta store the data as floats, they won't be clamped to 0..1
	fboSettings.textureTarget = GL_TEXTURE_2D;
	fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
	fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
	fboSettings.minFilter = GL_NEAREST; // No interpolation, that would mess up data reads later!
	fboSettings.maxFilter = GL_NEAREST;
	
	ofDisableTextureEdgeHack();
	
		particleDataFbo.allocate( fboSettings );
	
	ofEnableTextureEdgeHack();
	
	//	Buffer 1: XYZ pos, W age
	//	Buffer 2: XYZ vel

	// Initialise the starting and static data
	ofVec4f* startPositionsAndAge = new ofVec4f[numParticles];
	
	// We also create a vbo that has the texture coordinate for each particle's data
	particlePoints.setMode( OF_PRIMITIVE_POINTS );
	
	int tmpIndex = 0;
	for( int y = 0; y < textureSize; y++ )
	{
		for( int x = 0; x < textureSize; x++ )
		{
			float size = 0;
			ofVec3f pos (MathUtils::randomPointOnSphere() * 0.1);
			pos.set( ofRandom(-1,1), ofRandom(0,2), ofRandom(-1,1) );
			float startAge = ofRandom( particleMaxAge );
			
			startPositionsAndAge[tmpIndex] = ofVec4f( pos.x, pos.y, pos.z, startAge );
			
			ofVec2f texCoord;
			texCoord.x = ofMap( x + 0.5f,	0, textureSize,	0.0f, 1.0f ); // the original source has a  '+ 0.5' in it, to get the ceil?
			texCoord.y = ofMap( y + 0.5f,	0, textureSize,	0.0f, 1.0f );
			
			particlePoints.addVertex( ofVec3f(x,y,0) ); // this cuould be 0,0,0 as we won't use it, but put somehting in here so you can draw it without a shader to confirm it draws
			particlePoints.addTexCoord( texCoord );
			tmpIndex++;
		}
	}

	// Upload it to the source texture
	particleDataFbo.source()->getTextureReference(0).loadData( (float*)&startPositionsAndAge[0].x,	 textureSize, textureSize, GL_RGBA );

}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::update( float _time, float _timeStep )
{
	ofEnableBlendMode( OF_BLENDMODE_DISABLED ); // Important! We just want to write the data as is to the target fbo
	
	particleDataFbo.dest()->begin();
	
		particleDataFbo.dest()->activateAllDrawBuffers(); // if we have multiple color buffers in our FBO we need this to activate all of them
		
		particleUpdate.begin();
	
			particleUpdate.setUniformTexture( "u_positionAndAgeTex",	particleDataFbo.source()->getTextureReference(0), 0 );
			//particleUpdate.setUniformTexture( "velocityTex",		particleDataFbo.source()->getTextureReference(1), 1 );
			
			particleUpdate.setUniform1f("u_time", _time );
			particleUpdate.setUniform1f("u_timeStep", _timeStep );
			
			particleUpdate.setUniform1f("u_particleMaxAge", particleMaxAge );
			
			particleUpdate.setUniform1f("u_noisePositionScale", noisePositionScale );
			particleUpdate.setUniform1f("u_noiseTimeScale", noiseTimeScale );
			particleUpdate.setUniform1f("u_noisePersistence", noisePersistence );
			particleUpdate.setUniform1f("u_noiseMagnitude", noiseMagnitude );
			particleUpdate.setUniform3f("u_wind", baseSpeed.get().x, baseSpeed.get().y, baseSpeed.get().z );
			
			particleDataFbo.source()->draw(0,0);
		
		particleUpdate.end();
		
	particleDataFbo.dest()->end();
	
	particleDataFbo.swap();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::draw( ofCamera* _camera )
{
	ofFloatColor particleStartCol = startColor.get();
	ofFloatColor particleEndCol = endColor.get();

	ofSetColor( ofColor::white );
	ofEnableBlendMode( OF_BLENDMODE_ADD );
	//ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	ofEnablePointSprites();
	
	particleDrawUnsorted.begin();

		particleDrawUnsorted.setUniform2f("u_resolution", particleImage.getWidth(), particleImage.getHeight() );

		particleDrawUnsorted.setUniformTexture("u_particleImageTexture", particleImage.getTextureReference(), 0 );
		particleDrawUnsorted.setUniformTexture("u_particleDataTexture", particleDataFbo.source()->getTextureReference(), 1 );
		
		particleDrawUnsorted.setUniformMatrix4f("u_viewMatrix", _camera->getModelViewMatrix() );
		particleDrawUnsorted.setUniformMatrix4f("u_projectionMatrix", _camera->getProjectionMatrix() );
		particleDrawUnsorted.setUniformMatrix4f("u_modelViewProjectionMatrix", _camera->getModelViewProjectionMatrix() );

		particleDrawUnsorted.setUniform1f("u_particleMaxAge", particleMaxAge );
	
		particleDrawUnsorted.setUniform1f("u_particleDiameter", particleSize );
		particleDrawUnsorted.setUniform1f("u_screenWidth", ofGetWidth() );

		particleDrawUnsorted.setUniform4fv("u_particleStartColor", particleStartCol.v );
		particleDrawUnsorted.setUniform4fv("u_particleEndColor", particleEndCol.v );

		particlePoints.draw();

	particleDrawUnsorted.end();

	ofDisablePointSprites();
}