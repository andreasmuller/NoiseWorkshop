//
//  ofxWalkingFirstPersonCameraOculus.h
//  GaiaTerrain
//
//  Created by Andreas Müller on 02/02/2014.
//
//

#pragma once 

#include "ofxWalkingFirstPersonCamera.h"

class ofxWalkingFirstPersonCameraOculus : public ofxWalkingFirstPersonCamera
{
	public:

		// ----------------------------------------------
		ofxWalkingFirstPersonCameraOculus()
		{
			constrainToYAxis = true;
			disableAutoUpdate();
		}
	
		// ----------------------------------------------
		void update()
		{
			updateRotationMouse( constrainToYAxis );
			updateTranslationBasedOnHeadsetDirection();
			updatePhysics();
		}
	
		// ----------------------------------------------
		void updateTranslationBasedOnHeadsetDirection()
		{
			// Translation
			ofVec3f currPos = getPosition();
			
			ofVec3f forward =  ofVec3f(0,0,1) * headsetOrientation * getOrientationQuat();
			ofVec3f sideways = ofVec3f(1,0,0) * headsetOrientation * getOrientationQuat();
			
			ofVec3f frameSpeed(0,0,0); // Todo: should we integrate over time as well?
			
			if( ofGetKeyPressed(forwardKey) ) 	{ frameSpeed -= forward * movementMaxSpeed; }
			if( ofGetKeyPressed(backwardKey) ) 	{ frameSpeed += forward * movementMaxSpeed; }
			
			if( ofGetKeyPressed(rightKey) ) 	{ frameSpeed -= sideways * movementMaxSpeed; }
			if( ofGetKeyPressed(leftKey) ) 		{ frameSpeed += sideways * movementMaxSpeed; }
			
			movementSpeed.interpolate( frameSpeed, movementInertia );
			
			setPosition( getPosition() + movementSpeed );
			movementSpeed *= movementDrag;
		}
	
		// ----------------------------------------------
		void getHeadsetOrientation()
		{
			return headsetOrientation;
		}
		
		// ----------------------------------------------
		void setHeadsetOrientation( ofQuaternion _headsetOrientation )
		{
			headsetOrientation = _headsetOrientation;
		}

		// ----------------------------------------------
		void setConstrainToYAxis( bool _constrainToYAxis )
		{
			constrainToYAxis = _constrainToYAxis;
		}
	
		// ----------------------------------------------
		void getConstrainToYAxis()
		{
			return constrainToYAxis;
		}
	
	protected:
	
		ofQuaternion headsetOrientation;
		bool constrainToYAxis;
};