#pragma once

#include "ofParameter.h"
#include "ofCamera.h"
#include "ofEvents.h"

/* 
 99% Memo's ofEasyCam, I just needed to change a few small things and was having trouvle doing it by subclassing.
 */

class ofEasyCamExt : public ofCamera
{
	public:
	
		ofEasyCamExt();
		~ofEasyCamExt();

		// TODO: this should be ofGetViewRect() eventually
		virtual void begin(ofRectangle viewport = ofGetCurrentViewport());
		void reset();

		//----------------------------------------
		// advanced functions

		void setTarget(const ofVec3f& target);
		void setTarget(ofNode& target);
		ofNode& getTarget();

		void setDistance(float distance);
		float getDistance() const;

		// drag is how quickly the camera picks up and slows down
		// it is a normalized value between 0-1
		void setDrag(float drag);
		float getDrag() const;
		// the translation key is the key used to switch between rotation and translation.
		// translation happens only when the key is pressed.
		void setTranslationKey(char key);
		char getTranslationKey();
		
		void setDollyKey(char _key);
		char getDollyKey();
	
		void setDollyForwardKey(char _key);
		char getDollyForwardKey();

		void setDollyBackwardKey(char _key);
		char getDollyBackwardKey();
	
		void setDollyImpulseMagnitude( float _impulseMagnitude );
		float getDollyImpulseMagnitude();
	
		void dollyImpulse( float _impulse );
		
		// enable or disable mouse input to navigate
		void enableMouseInput();
		void disableMouseInput();
		bool getMouseInputEnabled();
		
		void enableMouseMiddleButton();
		void disableMouseMiddleButton();
		bool getMouseMiddleButtonEnabled();
		
		void setAutoDistance(bool bAutoDistance);
		
	protected:
	
		void setDistance(float distance, bool save);

		ofNode target;
		
		bool bEnableMouseMiddleButton;
		bool bApplyInertia;
		bool bDoTranslate;
		bool bDoRotate;
		bool bValidClick;
		bool bInsideArcball;
		bool bMouseInputEnabled;
		bool bDistanceSet;
		bool bAutoDistance;
		float lastDistance;

		float drag;
		
		float xRot;
		float yRot;
		float zRot;
		
		float moveX;
		float moveY;
		float moveZ;
		
		float sensitivityXY;
		float sensitivityZ;
		float sensitivityRot;
		
		float rotationFactor;

		ofVec2f mouse;
		ofVec2f lastMouse;
		ofVec2f mouseVel;
	
		void updateRotation();
		void updateTranslation();
		void update(ofEventArgs & args);
		void updateMouse();
		
		char doTranslationKey;
		char doDollyKey;
		
		char dollyForwardKey;
		char dollyBackwardKey;
		
		float dollyImpulseAmount;
		float maxDollyImpulseSpeed;
	
		unsigned long lastTap;
			
		ofQuaternion curRot;  
		
		ofRectangle viewport;// having the  viewport saved localy will make it easier for all the needed maths dealing with viewport.
};
