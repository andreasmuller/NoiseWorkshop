#pragma once

//  ShadowMapLight.h
//
//  Created by James Acres on 12-08-15
//  http://www.jamesacres.com
//  http://github.com/jacres
//  @jimmyacres

// OF'ified by Andreas Muller

#include "ofMain.h"
#include "ofLightExt.h"
#include "Utils/FboPingPong.h"

class ShadowMapLight : public ofLightExt
{
	public:
	
		ShadowMapLight();
		
		void    setup( int shadowMapSize=1024, float fov=60.0f, float near=0.1f, float far=200.0f );
	
		void    createShadowMapFBO();
		
		void    beginShadowMap();
		void    endShadowMap();
	
		ofTexture& getShadowMapTexture();
	
		void    blurShadowMap( int _numPasses = 1 );
	
		void	debugDrawViewingVolume();
		void    debugDrawShadowMap( ofPoint _pos, float _w, float _h );
	
		ofMatrix4x4 getShadowMatrix( ofCamera &cam );
		ofMatrix4x4 getShadowMatrix( ofMatrix4x4 _modelViewMat );

		void    	setBlurParams( float factor, int _numPasses = 1 );
		void 		setFov( float _fov );
		float 		getFov();
		void		setNearFar( float _near, float _far );
		float 		getNear();
		float 		getFar();
	
		float       getLinearDepthScalar();
	
	protected:

		static const ofMatrix4x4 s_biasMat;
	
		float		m_fov;
		float		m_nearZ;
		float		m_farZ;
		float       m_linearDepthScalar;
	
		bool        m_bIsSetup;
	
		FboPingPong	m_shadowFbo;
	
		ofShader    m_blurHShader;
		ofShader    m_blurVShader;
		ofShader    m_linearDepthShader;
	
		ofMatrix4x4 m_viewMatrix;
		ofMatrix4x4 m_projectionMatrix;
	
		float       m_texelSize;
		int         m_shadowMapSize;
		float       m_blurFactor;
		int 		m_numBlurPasses;

};
