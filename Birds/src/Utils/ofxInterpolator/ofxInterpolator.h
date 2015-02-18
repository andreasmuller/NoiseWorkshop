//
//  ofxInterpolator.h
//  AeroDynamics
//
//  Created by Andreas Müller on 31/05/2013.
//
//

#pragma once

#include "ofMain.h"

#include "EasingEquations.h"

template <class T>
class ofxInterpolator
{
	public:
	
		// ---------------------------------------------	
		ofxInterpolator()
		{
			isInterpolating = false;
			
			backWhenDone = false;
			fireEventWhenDone = false;
			
			interpolateStartTime = 0.0f;
			interpolateEndTime	  = 0.0f;
			interpolateTime = 0.0f;
			
			hasValueBound = false;
		}
		
		// ---------------------------------------------
		void bind( T* _targetValue )
		{
			boundValue = _targetValue;
			hasValueBound = true;
			enableAutoTweenUpdate();
		}

		// Todo: override the = operator, allowing us to set the internal value by doing 'interpolator = 1.0f'
		// That could initialise an internal value that we use
		/* operator =
		{
			if( hasValueBound )
			{
				set bound value to input value
			}
			else
			{
				set _internalValue to input value
				bind( _&internalValue );
			}
		} 
		 
		we also want the other way around valuyeInMyProgram = interpolator
		 {
			return boundValue;
		 }
		 */
	
		// Todo: Implement firing events when done, going back and looping
	
		// ---------------------------------------------
		void interpolateTo( T _target,
						   float _time,
						   EasingEquations::EaseType _easeType = EasingEquations::EASE_LINEAR,
						   float _startDelay = 0.0f,
						   bool _backWhenDone = false,
						   bool _fireEventWhenDone = false )
		{
			float currTime = ofGetElapsedTimef();
			
			valStart = *boundValue;
			valTarget = _target;
			easeType = _easeType;

			interpolateTime = _time;			
			
			interpolateStartTime = currTime + _startDelay;
			interpolateEndTime	 = interpolateStartTime + interpolateTime;
			
			isInterpolating = true;
		}
			
		// ---------------------------------------------
		void enableAutoTweenUpdate()
		{
			ofAddListener(ofEvents().update, this, &ofxInterpolator::_update);
		}

		// ---------------------------------------------
		void disableAutoTweenUpdate()
		{
			ofRemoveListener(ofEvents().update, this, &ofxInterpolator::_update);
		}
	
		// ---------------------------------------------
		void update()
		{
			if( isInterpolating )
			{
				float currTime = ofGetElapsedTimef();
				
				if( currTime < interpolateStartTime )
				{
					*boundValue = valStart;
				}
				else if( currTime > interpolateEndTime )
				{
					*boundValue = valTarget;
					isInterpolating = false;
					
					/*if( fireEventWhenDone && !backWhenDone )
					{
						TweenProperties::tweenDoneEventArgs.eventID = eventID;
						TweenProperties::tweenDoneEventArgs.type = animType;
						ofNotifyEvent( TweenProperties::tweenDoneEvent, TweenProperties::tweenDoneEventArgs );
					}*/
				}
				else
				{
					float tmpFrac = (currTime - interpolateStartTime) / (interpolateEndTime - interpolateStartTime);
					tmpFrac = ofClamp( tmpFrac, 0.0f, 1.0f );
					tmpFrac = EasingEquations::ease( tmpFrac, easeType );
					*boundValue = valStart + ((valTarget-valStart) * tmpFrac);
					
					//cout << "*boundValue " << *boundValue << " valStart: " << valStart << " valTarget: " << valTarget << "	tmpFrac: " << tmpFrac << " result: " << (valStart + ((valTarget-valStart) * tmpFrac)) << endl;
					//cout << ((valTarget-valStart) * tmpFrac) << "	tmpFrac: " << tmpFrac << endl;
				}
			}
		}
		
		// ---------------------------------------------
		void _update(ofEventArgs &e)
		{
			update();
		}
	
	private:
	
		T* boundValue;
	
		T valStart;
		T valTarget;
	
		bool isInterpolating;
	
		float interpolateStartTime;
		float interpolateEndTime;
		float interpolateTime;
	
		EasingEquations::EaseType easeType;
		EasingEquations::EaseType easeTypeBack;
		
		bool backWhenDone;
		bool fireEventWhenDone;
	
		bool hasValueBound;
	
		T _internalValue; // currently unused
};

