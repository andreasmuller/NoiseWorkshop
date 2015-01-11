//
//  ofxWalkingFirstPersonCamera.h
//  GaiaTerrain
//
//  Created by Andreas Müller on 26/01/2014.
//
//

#pragma once

#include "ofxFirstPersonCamera.h"

class  ofxWalkingFirstPersonCamera: public ofxFirstPersonCamera
{
	public:

		// ----------------------------------------------
		ofxWalkingFirstPersonCamera()
		{
			//setGravity( -9.8f );
			setGravity( 0.0f );
			setGroundLevelY( 0.0f );
			setEyeHeight( 0.8f );
			//cout << "ofxWalkingFirstPersonCamera()" << endl;
		}
	
		// ----------------------------------------------
		void update()
		{
			ofxFirstPersonCamera::update();
			updatePhysics();
		}

		// ----------------------------------------------
		void updatePhysics()
		{
			ofVec3f currPos = getPosition();
			
			// Todo: better physics model
			currPos.y += gravity;

			// Todo: base collision check on planes
			if( currPos.y < groundLevelY + eyeHeight )
			{
				currPos.y = ofLerp( currPos.y, groundLevelY + eyeHeight, 0.5f );
			}
			
			setPosition( currPos );
		}

		// ----------------------------------------------
		void setGroundLevelY( float _groundLevelY )
		{
			groundLevelY = _groundLevelY;
		}
	
		// ----------------------------------------------
		void setEyeHeight( float _eyeHeight )
		{
			eyeHeight = _eyeHeight;
		}
	
		// ----------------------------------------------
		void setGravity( float _gravity )
		{
			gravity = _gravity;
		}
	
	protected:
		
		float gravity;
		float groundLevelY;
		float eyeHeight;

};