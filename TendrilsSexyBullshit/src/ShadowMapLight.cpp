//  ShadowMapLight.cpp
//
//  Created by James Acres on 12-08-15
//  http://www.jamesacres.com
//  http://github.com/jacres
//  @jimmyacres

#include "shadowMapLight.h"


// bias matrix to scale -1.0 .. 1.0 to 0 .. 1.0
const ofMatrix4x4 ShadowMapLight::s_biasMat = ofMatrix4x4( 	0.5, 0.0, 0.0, 0.0,
															0.0, 0.5, 0.0, 0.0,
															0.0, 0.0, 0.5, 0.0,
															0.5, 0.5, 0.5, 1.0 );

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ShadowMapLight::ShadowMapLight()
{
	m_fov = 60;
	m_nearZ = 1;
	m_farZ = 25;
	m_shadowMapSize = 1024;
	m_texelSize = 1.0f/1024.0f;
	m_blurFactor = 4.0f;
	m_bIsSetup = false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::setup( int shadowMapSize, float fov, float near, float far )
{
	setFov( fov );
	setNearFar( near, far );
	
    m_shadowMapSize = shadowMapSize;
    m_texelSize = 1.0f/shadowMapSize;
	
	createShadowMapFBO();

	m_blurHShader.load( "Shaders/ShadowMapping/GL2/Basic.vert", "Shaders/ShadowMapping/GL2/Gaussblur_h5.frag" );
	m_blurVShader.load( "Shaders/ShadowMapping/GL2/Basic.vert", "Shaders/ShadowMapping/GL2/Gaussblur_v5.frag" );
	
    m_linearDepthShader.load( "Shaders/ShadowMapping/GL2/LinearDepthBuffer" );
	
    ofLight::setup(); // call ofLight constructor
	
	m_bIsSetup = true;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::createShadowMapFBO()
{
	ofFbo::Settings shadowFboSettings;
	
	shadowFboSettings.width						= m_shadowMapSize;
	shadowFboSettings.height					= m_shadowMapSize;
	shadowFboSettings.useDepth					= true;
	shadowFboSettings.useStencil				= false;
	shadowFboSettings.depthStencilAsTexture		= true;
	shadowFboSettings.textureTarget				= GL_TEXTURE_2D;
	shadowFboSettings.internalformat			= GL_R32F;
	shadowFboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT32;
	shadowFboSettings.wrapModeHorizontal		= GL_CLAMP_TO_BORDER;
	shadowFboSettings.wrapModeVertical			= GL_CLAMP_TO_BORDER;
	shadowFboSettings.minFilter					= GL_LINEAR;
	shadowFboSettings.maxFilter					= GL_LINEAR;
	
	m_shadowFbo.allocate( shadowFboSettings );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::setBlurParams( float factor, int _numPasses )
{
    m_blurFactor = factor;
	m_numBlurPasses = _numPasses;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::beginShadowMap( bool _bindLinearDepthShader )
{
	m_shadowFbo.source()->begin();
	
		ofClear( ofColor::white );
		
		//glEnable(GL_CULL_FACE); // cull front faces - this helps with artifacts and shadows with exponential shadow mapping
		//glCullFace(GL_FRONT); //glCullFace(GL_BACK);
	
		ofEnableDepthTest();

		if( _bindLinearDepthShader ) m_linearDepthShader.begin();

			if( _bindLinearDepthShader ) m_linearDepthShader.setUniform1f( "u_LinearDepthConstant", m_linearDepthScalar );
	
			ofVec3f eye = getGlobalPosition();
			ofVec3f center = eye + getLookAtDir();
			ofVec3f up = ofVec3f(0.0f, 1.0f, 0.0f);
			
			m_viewMatrix.makeLookAtViewMatrix(eye, center, up); // make our look at view matrix
			
			// Make a projection matrix that we'll use to render a scene from our light's viewpoint
			m_projectionMatrix.makePerspectiveMatrix( m_fov, m_shadowFbo.source()->getWidth()/m_shadowFbo.source()->getHeight(), m_nearZ, m_farZ );
	
			// load the view and projection matrices + save our current ones so we can restore them when done
			ofSetMatrixMode( OF_MATRIX_PROJECTION );
			ofLoadMatrix( m_projectionMatrix );

			ofSetMatrixMode( OF_MATRIX_MODELVIEW );
			ofLoadMatrix( m_viewMatrix );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::endShadowMap( bool _unBindLinearDepthShader )
{
		if( _unBindLinearDepthShader ) m_linearDepthShader.end();
	
	m_shadowFbo.source()->end();
	
	blurShadowMap( m_numBlurPasses ); // blur our shadow map

	ofEnableDepthTest();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ofMatrix4x4 ShadowMapLight::getShadowMatrix( ofCamera &cam )
{
    return getShadowMatrix( cam.getModelViewMatrix() );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ofMatrix4x4 ShadowMapLight::getShadowMatrix( ofMatrix4x4 _modelViewMat )
{
	// create a transform matrix that we can use in our shader to do the following while rendering our objects:
	// - convert our vertex position from camera/view space back to world space (need inverse of the camera's modelviewmatrix)
	// - convert this world space vertex to light clip space (view and projection matrix from out light)
	// - convert this light clip space value from -1.0 .. +1.0 to 0.0 .. 1.0 so that we can use it as a texture lookup for the shadowmap texture
	
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( _modelViewMat );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * m_viewMatrix * m_projectionMatrix * s_biasMat;
	
    return shadowTransMatrix;	
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ofTexture& ShadowMapLight::getShadowMapTexture()
{
	return m_shadowFbo.source()->getTextureReference();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
float ShadowMapLight::getLinearDepthScalar()
{
    return m_linearDepthScalar;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::setFov( float _fov )
{
	m_fov = _fov;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
float ShadowMapLight::getFov()
{
	return m_fov;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::setNearFar( float _near, float _far )
{
	m_nearZ = _near;
	m_farZ = _far;
	m_linearDepthScalar = 1.0 / (m_farZ - m_nearZ); // this helps us remap depth values to be linear

}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
float ShadowMapLight::getNear()
{
	return m_nearZ;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
float ShadowMapLight::getFar()
{
	return m_farZ;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::blurShadowMap( int _numPasses )
{
	int w = m_shadowFbo.dest()->getWidth();
	int h = m_shadowFbo.dest()->getHeight();

	ofEnableBlendMode( OF_BLENDMODE_DISABLED );
	ofDisableDepthTest();
	ofSetColor( ofColor::white );

	for( int i = 0; i < _numPasses; i++ )
	{
			// draw the full viewport quad
		m_shadowFbo.dest()->begin();
		
			m_blurHShader.begin();
			
				m_blurHShader.setUniform1i( "blurSampler", 0 );
				m_blurHShader.setUniform1f( "sigma", m_blurFactor );
				m_blurHShader.setUniform1f( "blurSize", m_texelSize  );
		
				m_shadowFbo.source()->draw(0,0,w,h);
		
			m_blurHShader.end();
		
		m_shadowFbo.dest()->end();

		m_shadowFbo.swap();
		
		// vertical blur pass
		m_shadowFbo.dest()->begin();
		
			m_blurVShader.begin();
			
				m_blurVShader.setUniform1i( "blurSampler", 0 );
				m_blurVShader.setUniform1f( "sigma", m_blurFactor );
				m_blurVShader.setUniform1f( "blurSize", m_texelSize  );
		
				m_shadowFbo.source()->draw(0,0,w,h);
			
			m_blurVShader.end();
		
		m_shadowFbo.dest()->end();

		m_shadowFbo.swap();
	}
	
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::debugDrawViewingVolume()
{
	ofPushMatrix();
	
		// draw a small sphere at the light position
		ofDrawSphere( getGlobalPosition(), 0.04 );
		ofLine( getGlobalPosition(), getGlobalPosition() + (getLookAtDir() * 2.0) );
		
		ofMultMatrix( m_viewMatrix.getInverse() );
		ofMultMatrix( m_projectionMatrix.getInverse());
		
		ofPushStyle();
			ofNoFill();
			ofDrawBox(2.0f);
		ofPopStyle();
	
	ofPopMatrix();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ShadowMapLight::debugDrawShadowMap( ofPoint _pos, float _w = 0.0, float _h = 0.0 )
{
	float w = _w;
	float h = _h;
	if( w == 0.0 ) w = m_shadowFbo.source()->getWidth();
	if( h == 0.0 ) h = m_shadowFbo.source()->getHeight();
	m_shadowFbo.source()->draw( _pos.x, _pos.y, w, h );
}
