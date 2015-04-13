//
//  ParticleSystemGPU.cpp
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#include "ParticleSystemInstancedGeometryGPU.h"

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::init( int _texSize )
{
	string xmlSettingsPath = "Settings/Particles.xml";
	gui.setup( "Particles", xmlSettingsPath );
	gui.add( particleMaxAge.set("Particle Max Age", 10.0f, 0.0f, 20.0f) );
	gui.add( noiseMagnitude.set("Noise Magnitude", 0.075, 0.01f, 1.0f) );
	gui.add( noisePositionScale.set("Noise Position Scale", 1.5f, 0.01f, 5.0f) );
	gui.add( noiseTimeScale.set("Noise Time Scale", 1.0 / 4000.0, 0.001f, 1.0f) );
	gui.add( noisePersistence.set("Noise Persistence", 0.2, 0.001f, 1.0f) );
	gui.add( baseSpeed.set("Wind", ofVec3f(0.5,0,0), ofVec3f(-2,-2,-2), ofVec3f(2,2,2)) );
	gui.add( startColor.set("Start Color", ofColor::white, ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( endColor.set("End Color", ofColor(0,0,0,0), ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	//gui.add( twistNoiseTimeScale.set("Twist Noise Time Scale", 0.01, 0.0f, 0.5f) );
	//gui.add( twistNoisePosScale.set("Twist Noise Pos Scale", 0.25, 0.0f, 2.0f) );
	//gui.add( twistMinAng.set("Twist Min Ang", -1, -5, 5) );
	//gui.add( twistMaxAng.set("Twist Max Ang", 2.5, -5, 5) );
	
	gui.add( materialShininess.set("Material Shininess",  20,  0, 127) );
	gui.add( materialAmbient.set("Material Ambient",   	 ofColor(50,50,50), 	ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( materialSpecular.set("Material Specular",   ofColor(255,255,255),  ofColor(0,0,0,0), ofColor(255,255,255,255)) );
	gui.add( materialEmissive.set("Material Emmissive",  ofColor(0,0,0),  ofColor(0,0,0,0), ofColor(255,255,255,255)) );

	gui.loadFromFile( xmlSettingsPath );
	gui.minimizeAll();
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	// Load shaders
	particleUpdate.load("Shaders/Particles/GL2/Update");
	particleDraw.load("Shaders/Particles/GL2/DrawInstancedGeometry");

	// Set how many particles we are going to have, this is based on data texture size
	textureSize = 128;
	numParticles = textureSize * textureSize;

	// Allocate buffers
	ofFbo::Settings fboSettings;
	fboSettings.width  = textureSize;
	fboSettings.height = textureSize;
	
	// We can create several color buffers for one FBO if we want to store velocity for instance,
	// then draw to them simultaneously from a shader using gl_FragData[0], gl_FragData[1], etc.
	fboSettings.numColorbuffers = 2;
	
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
	
	// We are going to encode our data into the FBOs like this
	//
	//	Buffer 1: XYZ pos, W age
	//	Buffer 2: XYZ vel, W not used
	//
	
	// Initialise the starting and static data
	ofVec4f* startPositionsAndAge = new ofVec4f[numParticles];
	
	int tmpIndex = 0;
	for( int y = 0; y < textureSize; y++ )
	{
		for( int x = 0; x < textureSize; x++ )
		{
			ofVec3f pos(0,0,0);
			//ofVec3f pos (MathUtils::randomPointOnSphere() * 0.1);
			//pos.set( ofRandom(-1,1), ofRandom(0,2), ofRandom(-1,1) );
			float startAge = ofRandom( particleMaxAge ); // position is not very important, but age is, by setting the lifetime randomly somewhere in the middle we can get a steady stream emitting
			
			startPositionsAndAge[tmpIndex] = ofVec4f( pos.x, pos.y, pos.z, startAge );
			
			tmpIndex++;
		}
	}

	// Upload it to the source texture
	particleDataFbo.source()->getTextureReference(0).loadData( (float*)&startPositionsAndAge[0].x,	 textureSize, textureSize, GL_RGBA );

	ofPrimitiveMode primitiveMode = OF_PRIMITIVE_TRIANGLES; // as we'll be drawing ths mesh instanced many times, we need to have many triangles as opposed to one long triangle strip
	ofMesh tmpMesh;
	
	ofConePrimitive cone( 0.1, 0.1,  5, 2, primitiveMode );
	//tmpMesh = cone.getMesh();

	ofBoxPrimitive box( 0.0015, 0.0015,  0.01 ); // we gotta face in the -Z direction
	tmpMesh = box.getMesh();
	
	singleParticleMesh.append( tmpMesh );
	singleParticleMesh.setMode( primitiveMode );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::update( float _time, float _timeStep )
{
	particleMaterial.setAmbientColor( materialAmbient.get() );
	particleMaterial.setSpecularColor( materialSpecular.get() );
	particleMaterial.setEmissiveColor( materialEmissive.get() );
	particleMaterial.setShininess( materialShininess );
	
	updateParticles( _time, _timeStep );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::draw( ofCamera* _camera )
{
	drawParticles( &particleDraw, _camera );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::drawGui()
{
	gui.draw();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::updateParticles( float _time, float _timeStep )
{
	ofEnableBlendMode( OF_BLENDMODE_DISABLED ); // Important! We just want to write the data as is to the target fbo
	
	particleDataFbo.dest()->begin();
	
		particleDataFbo.dest()->activateAllDrawBuffers(); // if we have multiple color buffers in our FBO we need this to activate all of them
		
		particleUpdate.begin();
	
			particleUpdate.setUniformTexture( "u_particlePosAndAgeTexture",	particleDataFbo.source()->getTextureReference(0), 0 );
			particleUpdate.setUniformTexture( "u_particleVelTexture",		particleDataFbo.source()->getTextureReference(1), 1 );
			
			particleUpdate.setUniform1f("u_time", _time );
			particleUpdate.setUniform1f("u_timeStep", _timeStep );
			
			particleUpdate.setUniform1f("u_particleMaxAge", particleMaxAge );
			
			particleUpdate.setUniform1f("u_noisePositionScale", noisePositionScale );
			particleUpdate.setUniform1f("u_noiseTimeScale", noiseTimeScale );
			particleUpdate.setUniform1f("u_noisePersistence", noisePersistence );
			particleUpdate.setUniform1f("u_noiseMagnitude", noiseMagnitude );
			particleUpdate.setUniform3f("u_baseSpeed", baseSpeed.get().x, baseSpeed.get().y, baseSpeed.get().z );
			
			particleDataFbo.source()->draw(0,0);
		
		particleUpdate.end();
		
	particleDataFbo.dest()->end();
	
	particleDataFbo.swap();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::drawParticles( ofShader* _shader, ofCamera* _camera )
{
	ofFloatColor particleStartCol = startColor.get();
	ofFloatColor particleEndCol = endColor.get();

	ofSetColor( ofColor::white );
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	_shader->begin();

		_shader->setUniformTexture("u_particlePosAndAgeTexture", particleDataFbo.source()->getTextureReference(0), 1 );
		_shader->setUniformTexture("u_particleVelTexture", particleDataFbo.source()->getTextureReference(1), 2 );
	
		_shader->setUniform2f("u_resolution", particleDataFbo.source()->getWidth(), particleDataFbo.source()->getHeight() );
		_shader->setUniform1f("u_time", ofGetElapsedTimef() );
	
		_shader->setUniformMatrix4f("u_modelViewMatrix", _camera->getModelViewMatrix() );
		_shader->setUniformMatrix4f("u_projectionMatrix", _camera->getProjectionMatrix() );
		_shader->setUniformMatrix4f("u_modelViewProjectionMatrix", _camera->getModelViewProjectionMatrix() );

		_shader->setUniform1f("u_particleMaxAge", particleMaxAge );

		_shader->setUniform1i("u_numLights", 1 );
	
		_shader->setUniform4fv("u_particleStartColor", particleStartCol.v );
		_shader->setUniform4fv("u_particleEndColor", particleEndCol.v );

		// Calling begin() on the material sets the OpenGL state that we then read in the shader
		particleMaterial.begin();
	
			singleParticleMesh.drawInstanced( OF_MESH_FILL, numParticles );

		particleMaterial.end();
	
	_shader->end();
	
}
