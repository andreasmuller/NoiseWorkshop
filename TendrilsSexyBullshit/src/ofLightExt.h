//
//  ofLightExt.h
//  GeometryShaderTubes
//
//  Created by Andreas Müller on 06/12/2014.
//
//


#pragma once

#include "ofLight.h"

// ------------------------------------------------------------------------
//
class ofLightExt : public ofLight
{
public:

	// --------------------------------------------
	ofLightExt()
	{
		radius = 10.0f;
	}
	
	// --------------------------------------------
	static void setParams( ofShader* _shader, vector<ofLightExt*>& _lights, ofCamera* _camera )
	{
		int numActiveLights = 0;
		
		// Todo: in oF 0.8.4 we can't grab the global ambient, fix
		ofFloatColor lightSceneAmbient = ofFloatColor::black; // ofGetGlobalAmbientColor();

		vector<ofVec3f> lightPositionWorld;
		vector<ofVec3f> lightPositionCamera;
		vector<ofFloatColor> lightDiffuse;
		vector<ofFloatColor> lightSpecular;
		vector<float> lightRadius;
		
		//ofLogNotice() << " _lights.size() " << _lights.size();
		
		for( unsigned int i = 0; i < _lights.size(); i++ )
		{
			ofLightExt* light = _lights.at(i);
			
			//ofLogNotice() << "Light " <<  i << " light->getIsEnabled() " << light->getIsEnabled();
			
			if( /*light->getIsEnabled()*/ true ) // getIsEnabled() is broken in of 0.8.4, so for now just assume that all lights are enabled
			{
				lightPositionWorld.push_back( light->getGlobalPosition() );
				lightPositionCamera.push_back( light->getGlobalPosition() * _camera->getModelViewMatrix() );
				
				lightDiffuse.push_back( light->getDiffuseColor() );
				lightSpecular.push_back( light->getSpecularColor() );
				lightRadius.push_back( light->getRadius() );
				
				light->enable(); // Call this again to set latest parameters, for GL2 in case a shader is using those built-ins.
				
				numActiveLights++;
			}
		}
		
		//ofLogNotice() << "numActiveLights: " << numActiveLights;

		_shader->setUniform1i("numActiveLights", numActiveLights );
		
		_shader->setUniform4fv("lightSceneAmbient", (float*)&lightSceneAmbient.v );
		
		if( numActiveLights > 0 )
		{
			// TODO: look into this, are we getting our uniforms through?
			_shader->setUniform3fv("lightPositionWorld", 	lightPositionWorld.at(0).getPtr(), 	numActiveLights );
			_shader->setUniform3fv("lightPositionCamera", 	lightPositionCamera.at(0).getPtr(), numActiveLights );
			_shader->setUniform4fv("lightDiffuse", 			(float*)&lightDiffuse.at(0).v[0], 	numActiveLights );
			_shader->setUniform4fv("lightSpecular", 		(float*)&lightSpecular.at(0).v[0], 	numActiveLights );
			_shader->setUniform1fv("lightRadius", 			(float*)&lightRadius.at(0), 		numActiveLights );
		}
		
		/*
		for( unsigned int i = 0; i < lightSpecular.size(); i++ )
		{
			ofLogNotice() << "- - - - -";
			ofLogNotice() << lightDiffuse.at(i);
			ofLogNotice() << lightSpecular.at(i);
			ofLogNotice() << lightRadius.at(i);
		}
		 */
		
	}

	// --------------------------------------------
	void draw( float _size = 0.2 )
	{
		ofSetColor( getDiffuseColor() );
		ofDrawSphere( getGlobalPosition(), _size );
		
		//ofSetColor( getDiffuseColor(), 5 );
		//ofDrawSphere( getGlobalPosition(), getRadius() );
	}

	// --------------------------------------------
	void setRadius( float _radius )
	{
		radius = _radius;
	}

	// --------------------------------------------
	float getRadius()
	{
		return radius;
	}


	float radius;
};
