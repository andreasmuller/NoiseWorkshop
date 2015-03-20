#pragma once

#include "ofMain.h"

class MathUtils
{
	
	public:
		
		// -------------------------------------------
		static float step(float a, float x)
		{
			return (float) (x >= a);
		}
	
		// -------------------------------------------
		static float linearStep( float _edge0, float _edge1, float _t )
		{
			// Scale, and clamp x to 0..1 range
			return ofClamp( (_t - _edge0)/(_edge1 - _edge0), 0.0f, 1.0f);
		}
	
		// -------------------------------------------
		static float linearStepInOut( float _t, float _low0, float _high0, float _high1, float _low1 )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			
			float localLow1  = _low1;
			float localHigh1 = _high1;
			
			return linearStep( localLow0, localHigh0, _t ) * (1.0f - linearStep( localHigh1, localLow1, _t ));
		}

		// ------------------------------------------------------------
		static float smoothstep(float edge0, float edge1, float x)
		{
			// Scale, and clamp x to 0..1 range
			x = ofClamp( (x - edge0)/(edge1 - edge0), 0, 1);
			// Evaluate polynomial
			return x*x*x*(x*(x*6 - 15) + 10);
		}
	
		// -------------------------------------------
		static float smoothStepInOut( float _t, float _low0, float _high0, float _high1, float _low1 )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			
			float localLow1  = _low1;
			float localHigh1 = _high1;
			
			return smoothstep( localLow0, localHigh0, _t ) * (1.0f - smoothstep( localHigh1, localLow1, _t ));
		}

		// ------------------------------------------------------------
		static float pingPong( float _value, float _period )
		{
			return abs( _period - fmodf(_value, (2*_period)) );
		}
	
		// ------------------------------------------------------------
		static float cosCurve( float _valueIn, float _maxValue = 1.0f )
		{
			float tmpVal = _valueIn / _maxValue;
			tmpVal = cosf( tmpVal * (PI*2.0f) );
			tmpVal += 1.0f;
			tmpVal /= 2.0f;
			
			return tmpVal;
		}
		
		// ------------------------------------------------------------
		static float sinCurve( float _valueIn, float _maxValue = 1.0f )
		{
			float tmpVal = _valueIn / _maxValue;
			tmpVal = sinf( tmpVal * (PI*2.0f) );
			tmpVal += 1.0f;
			tmpVal /= 2.0f;
			
			return tmpVal;
		}

		// ------------------------------------------------------------
		static ofVec3f randomPointOnUnitSphere()
		{
			ofVec3f normal;
			
			float azimuthal = ofRandom( PI * 2.0f );
			float zenith = asin(sqrt(ofRandom(1.0f)));
			
			// Calculate the cartesian point
			normal.x = sin(zenith)*cos(azimuthal);
			normal.y = sin(zenith)*sin(azimuthal);
			normal.z = cos(zenith);
			
			return normal;
		}

		// ------------------------------------------------------------
		static float getTriangleArea( ofVec3f _p0, ofVec3f _p1, ofVec3f _p2 )
		{
			ofVec3f triangle1Side1 = _p0 - _p1;
			ofVec3f triangle1Side2 = _p0 - _p2;
			ofVec3f triangle1Side3 = _p1 - _p2;
			float P1 = (triangle1Side1.length() + triangle1Side2.length() + triangle1Side3.length() ) / 2.0f;
			return (float)sqrt(P1 * (P1-triangle1Side1.length()) * (P1-triangle1Side2.length()) * (P1-triangle1Side3.length()) );
		}
	
		// ------------------------------------------------------------
		static float horizontalToVerticalFov(float _horizontalfov, float _aspect ) // aspect = w/h
		{
			_horizontalfov = ofDegToRad(_horizontalfov);
			float yfov = 2.0f * atanf(tanf(_horizontalfov * 0.5f) / _aspect);
			return ofRadToDeg(yfov);
		}
	
		// ------------------------------------------------------------
		static float verticalToHorizontalFov(float _verticalFov, float _aspect)
		{
			_verticalFov = ofDegToRad(_verticalFov);
			float xfov = 2.0f * atanf(tanf(_verticalFov * 0.5f) * _aspect);
			return ofRadToDeg(xfov);
		}

		// ------------------------------------------------------------	
		static unsigned int permuteQPR(unsigned int x)
		{
			static const unsigned int prime = 4294967291u;
			if (x >= prime)
				return x;  // The 5 integers out of range are mapped to themselves.
			unsigned int residue = ((unsigned long long) x * x) % prime;
			return (x <= prime / 2) ? residue : prime - residue;
		}

		// ------------------------------------------------------------
		static bool isInsideEllipse( ofVec2f _p, ofRectangle _ellipseRectangle )
		{
			if( _ellipseRectangle.inside( _p ) )
			{
				ofVec2f center = _ellipseRectangle.getPosition() + (ofVec2f(_ellipseRectangle.getWidth(), _ellipseRectangle.getHeight()) * 0.5f);
				
				float _xRadius = _ellipseRectangle.width * 0.5f;
				float _yRadius = _ellipseRectangle.height * 0.5f;
				
				if (_xRadius <= 0.0 || _yRadius <= 0.0)
					return false;
				
				ofVec2f normalized = ofVec2f(_p.x - center.x,
											 _p.y - center.y);
				
				return ((normalized.x * normalized.x) / (_xRadius * _xRadius)) + ((normalized.y * normalized.y) / (_yRadius * _yRadius))
						<= 1.0;
			}
			
			return false;
		}
	
	private:
};