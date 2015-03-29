//
//  FirstPersonCamera.h
//  GaiaTerrain
//
//  Created by Andreas Müller on 24/01/2014.
//
//

#pragma once

#include "ofMain.h"

#include "ofAppGLFWWindow.h"

class ofxFirstPersonCamera : public ofCamera
{
	public:
	
		// ----------------------------------------------
		ofxFirstPersonCamera()
		{
			forwardKey 	= 'w';
			backwardKey = 's';
			rightKey 	= 'a';
			leftKey 	= 'd';
			upKey		= 'q';
			downKey		= 'z';
		
			movementMaxSpeed = 0.1f;
			movementInertia = 0.15f;
			movementDrag = 0.99f;

			mouseSensitivity = 0.3f;
			rotationInertia = 0.2f;
			rotationDrag = 0.8f;

			lastMouse = ofVec2f( ofGetMouseX(), ofGetMouseY() );
			
			prevFrameNumReadJoystick = 0;
			
			useJoystick = false;
			axisRotateX = 3;
			axisRotateY = 2;
			axisMoveForwards = 1;
			axisMoveSideways = 0;
			axisDeadZone = 0.15f;
			rotationJoystickInertia = 0.4f;
			
			autoUpdate = false;
			enableAutoUpdate();
		}

		// ----------------------------------------------
		~ofxFirstPersonCamera()
		{
			disableAutoUpdate();
		}
	
		// ----------------------------------------------
		void enableAutoUpdate()
		{
			ofAddListener(ofEvents().update , this, &ofxFirstPersonCamera::update);
			autoUpdate = true;
		}

		// ----------------------------------------------
		void disableAutoUpdate()
		{
			if( autoUpdate )
			{
				ofRemoveListener(ofEvents().update, this, &ofxFirstPersonCamera::update);
				autoUpdate = false;
			}
		}

		// ----------------------------------------------
		virtual void update()
		{
			updateRotationMouse();
			updateTranslationKeyboard();
			
			if( useJoystick )
			{
				updateRotationJoystick();
				updateTranslationJoystick();
			}
		}
	
		// ----------------------------------------------
		void updateRotationMouse( bool _constrainToYAxis = false )
		{
			ofVec2f mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
			ofVec2f mouseVel = mouse - lastMouse;
			lastMouse = mouse;
			
			// Rotation
			rotationSpeed *= rotationDrag;
			if( rotationSpeed.length() < 0.00000001f ) { rotationSpeed = ofVec3f(0,0,0); }
			
			if( ofGetMousePressed( OF_MOUSE_BUTTON_RIGHT) )
			{
				rotationSpeed.x = ofLerp( rotationSpeed.x,  mouseVel.y * mouseSensitivity, rotationInertia );
				rotationSpeed.y = ofLerp( rotationSpeed.y,  mouseVel.x * mouseSensitivity, rotationInertia );
			}
			
			ofQuaternion tmpRotX( rotationSpeed.x, ofVec3f(1,0,0));
			ofQuaternion tmpRotY( rotationSpeed.y, ofVec3f(0,1,0));
			
			// Todo: neater solution to this
			if( _constrainToYAxis ) { setOrientation( getOrientationQuat() * tmpRotY ); }
			else { setOrientation( tmpRotX * getOrientationQuat() * tmpRotY ); }
		}

		// ----------------------------------------------
		void updateTranslationKeyboard()
		{
			// Translation
			ofVec3f currPos = getPosition();
			
			ofVec3f frameSpeed(0,0,0); // Todo: should we integrate over time as well?
			
			if( ofGetKeyPressed(forwardKey) ) 	{ frameSpeed -= getZAxis() * movementMaxSpeed; }
			if( ofGetKeyPressed(backwardKey) ) 	{ frameSpeed += getZAxis() * movementMaxSpeed; }
			
			if( ofGetKeyPressed(rightKey) ) 	{ frameSpeed -= getXAxis() * movementMaxSpeed; }
			if( ofGetKeyPressed(leftKey) ) 		{ frameSpeed += getXAxis() * movementMaxSpeed; }
			
			//cout << "getYAxis(): " << getYAxis();
			
			if( ofGetKeyPressed(upKey) ) 		{ frameSpeed += getYAxis() * movementMaxSpeed; }
			if( ofGetKeyPressed(downKey) ) 		{ frameSpeed -= getYAxis() * movementMaxSpeed; }

			movementSpeed.interpolate( frameSpeed, movementInertia );
			
			setPosition( getPosition() + movementSpeed );
			movementSpeed *= movementDrag;
		}
	
		// ----------------------------------------------
		void updateTranslationJoystick()
		{
			// Translation
			ofVec3f currPos = getPosition();
			
			ofVec3f frameSpeed(0,0,0); // Todo: should we integrate over time as well?
			
			float joystickAxisReadingForward  = getJoystickAxis( axisMoveForwards, axisDeadZone );
			float joystickAxisReadingSideways = getJoystickAxis( axisMoveSideways, axisDeadZone );
			
			frameSpeed -= getZAxis() * -joystickAxisReadingForward  * movementMaxSpeed;
			frameSpeed -= getXAxis() * -joystickAxisReadingSideways * movementMaxSpeed;

			movementSpeed.interpolate( frameSpeed, movementInertia );
			
			setPosition( getPosition() + movementSpeed );
			//movementSpeed *= movementDrag; Todo: we only want to do this once for both
		}
	
		// ----------------------------------------------
		void updateRotationJoystick( bool _constrainToYAxis = false )
		{
			float joystickSensitivity = 1.0f;
			
			// Rotation
			rotationJoystickSpeed *= rotationDrag;
			if( rotationJoystickSpeed.length() < 0.00000001f ) { rotationJoystickSpeed = ofVec3f(0,0,0); }
			
			float joystickAxisReadingX = -getJoystickAxis( axisRotateX, axisDeadZone );
			float joystickAxisReadingY = -getJoystickAxis( axisRotateY, axisDeadZone );
			
			if( abs(joystickAxisReadingX) > 0.0f ) rotationJoystickSpeed.x = ofLerp( rotationJoystickSpeed.x, joystickAxisReadingX * joystickSensitivity, 			rotationJoystickInertia );
			if( abs(joystickAxisReadingY) > 0.0f ) rotationJoystickSpeed.y = ofLerp( rotationJoystickSpeed.y, joystickAxisReadingY * joystickSensitivity * 2.0f, 	rotationJoystickInertia );

			ofQuaternion tmpRotX( rotationJoystickSpeed.x, ofVec3f(1,0,0));
			ofQuaternion tmpRotY( rotationJoystickSpeed.y, ofVec3f(0,1,0));
			
			// Todo: neater solution to this
			if( _constrainToYAxis ) { setOrientation( getOrientationQuat() * tmpRotY ); }
			else { setOrientation( tmpRotX * getOrientationQuat() * tmpRotY ); }
		}
	
		// ----------------------------------------------
		char setForwardKey(  char _forwardKey ) { forwardKey 	= _forwardKey; }
		char setBackwardKey( char _backwardKey) { backwardKey 	= _backwardKey; }
		char setRightKey( 	 char _rightKey) 	{ rightKey 		= _rightKey; }
		char setLeftKey( 	 char _leftKey) 	{ leftKey 		= _leftKey; }
	
		// ----------------------------------------------
		char getForwardKey() 	{ return forwardKey; }
		char getBackwardKey() 	{ return backwardKey; }
		char getRightKey() 		{ return rightKey; }
		char getLeftKey() 		{ return leftKey; }

		// ----------------------------------------------
		void setMovementMaxSpeed( float _movementMaxSpeed )
		{
			movementMaxSpeed = _movementMaxSpeed;
		}

		// ----------------------------------------------
		void setUseJoystick( bool _useJoystick )
		{
			useJoystick = _useJoystick;
		}
	
		// ----------------------------------------------
		bool getUseJoystick()
		{
			return useJoystick;
		}
		
	protected:

		// ----------------------------------------------
		void update(ofEventArgs & args)
		{
			update();
		}

		// ----------------------------------------------
		float getJoystickAxis( int _axis, float _deadZone = 0.0f )
		{
			pollJoystick(); // skips polling if we already polled this frame
			
			if( _axis < joystickAxis.size() )
			{
				float axisReading = joystickAxis.at( _axis );
				int sign = ofSign( axisReading );
	
				return ofMap( ofClamp( abs(axisReading) - _deadZone, 0.0f, 1.0f),   0.0f, 1.0f - _deadZone, 0.0f, 1.0f ) * sign;
			}
			
			return 0.0f;
		}
	
		// ----------------------------------------------
		bool isJoystickButtonPressed( int _button )
		{
			pollJoystick(); // skips polling if we already polled this frame
			
			if( _button < joystickButtons.size() )
			{
				return joystickButtons.at( _button );
			}

			return false;
		}

		// ----------------------------------------------
		void pollJoystick()
		{
			if( prevFrameNumReadJoystick == ofGetFrameNum() )
			{
				return;
			}
			
			int joystickIndex = 0;
			if (glfwJoystickPresent( joystickIndex ) == GL_TRUE)
			{
				int numButtons, numAxes;
				const unsigned char *pressed = glfwGetJoystickButtons(joystickIndex, &numButtons);
				const float *axes = glfwGetJoystickAxes(joystickIndex, &numAxes);
				//printf("%d %d -- %p %p\n", nbuttons, naxes, pressed, axes);
				
				joystickButtons.clear();
				for( int i = 0; i < numButtons; i++ ) { joystickButtons.push_back( pressed[i] ); }
				
				joystickAxis.clear();
				for( int i = 0; i < numAxes; i++ ) { joystickAxis.push_back( axes[i] ); }
			}
			
			prevFrameNumReadJoystick = ofGetFrameNum();
		}
		
		bool useJoystick;
		int prevFrameNumReadJoystick;
		float axisDeadZone;
		vector<float> joystickAxis;
		vector<bool> joystickButtons;
		int axisRotateX;
		int axisRotateY;
		int axisMoveForwards;
		int axisMoveSideways;
		float rotationJoystickInertia;
		ofVec2f rotationJoystickSpeed;
	
	
		bool autoUpdate;
		
		float mouseSensitivity;
		float rotationInertia;
		float rotationDrag;
		ofVec2f rotationSpeed;
	
		float movementMaxSpeed;
		float movementInertia;
		float movementDrag;
		ofVec3f movementSpeed;
	
		ofVec2f lastMouse;
	
		char forwardKey;
		char backwardKey;
		char rightKey;
		char leftKey;
		char upKey;
		char downKey;
};