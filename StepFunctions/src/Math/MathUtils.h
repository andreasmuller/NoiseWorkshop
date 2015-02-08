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
		static float linearStepInOut( float _low0, float _high0, float _high1, float _low1, float _t )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			
			float localLow1  = _low1;
			float localHigh1 = _high1;
			
			return linearStep( localLow0, localHigh0, _t ) * (1.0f - linearStep( localHigh1, localLow1, _t ));
		}

		// ------------------------------------------------------------
		static float smoothStep(float edge0, float edge1, float x)
		{
			// Scale, and clamp x to 0..1 range
			x = ofClamp( (x - edge0)/(edge1 - edge0), 0, 1);
			// Evaluate polynomial
			return x*x*x*(x*(x*6 - 15) + 10);
		}
	
		// -------------------------------------------
		static float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t )
		{
			float localLow0  = _low0;
			float localHigh0 = _high0;
			
			float localLow1  = _low1;
			float localHigh1 = _high1;
			
			return smoothStep( localLow0, localHigh0, _t ) * (1.0f - smoothStep( localHigh1, localLow1, _t ));
		}

		// ------------------------------------------------------------
		static float pingPong( float _value, float _period )
		{
			return abs( _period - fmodf(_value, (2*_period)) );
		}
	
		// ------------------------------------------------------------
		static float Cosine( float _ang, float _outMin, float _outMax )
		{
			return ofMap( cosf(_ang), -1, 1, _outMin, _outMax );
		}
	
		// ------------------------------------------------------------
		static float Sine( float _ang, float _outMin, float _outMax )
		{
			return ofMap( sinf(_ang), -1, 1, _outMin, _outMax );
		}

		// ------------------------------------------------------------
		static float snap( float _value, float _snapInterval, float snapDeadZoneNormalized = 0.0f )
		{
			int timesInterval = _value / _snapInterval;
			float tmpIntervalFrac = ofMap( _value, timesInterval * _snapInterval, (timesInterval + 1) * _snapInterval, 0.0f, 1.0f );
		
			//snapDeadZoneNormalized
			tmpIntervalFrac = ofMap( tmpIntervalFrac, snapDeadZoneNormalized, 1 - snapDeadZoneNormalized,  0, 1);
			
			tmpIntervalFrac = MathUtils::smoothStep( 0, 1, tmpIntervalFrac );
			
			return (timesInterval * _snapInterval) + (tmpIntervalFrac * _snapInterval );
		}

		// ------------------------------------------------------------
		// From: http://www.iquilezles.org/www/articles/functions/functions.htm
		static float cubicPulse( float _freq, float _halfWidth, float _t )
		{
			_t = fmodf( _t, _freq * 2.0);
			
			_t = fabsf( _t - _freq );
			
			if( _t>_halfWidth ) return 0.0f;
			_t /= _halfWidth;
			return 1.0f - _t*_t*(3.0f-2.0f*_t);
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

		// ------------------------------------------------------------
		static ofVec3f randomPointOnSphere()
		{
			float lambda = ofRandom(1.0f);
			float u = ofRandom(-1.0f, 1.0f);
			float phi = ofRandom( 2.0 * PI );
			
			ofVec3f p;
			p.x = pow(lambda, 1/3) * sqrt(1.0 - u * u) * cos(phi);
			p.y = pow(lambda, 1/3) * sqrt(1.0 - u * u) * sin(phi);
			p.z = pow(lambda, 1/3) * u;
			
			return p;
		}
	/*
	var randomPointInSphere = function () {
		var lambda = Math.random();
		var u = Math.random() * 2.0 - 1.0;
		var phi = Math.random() * 2.0 * Math.PI;
		
		return [
				Math.pow(lambda, 1/3) * Math.sqrt(1.0 - u * u) * Math.cos(phi),
				Math.pow(lambda, 1/3) * Math.sqrt(1.0 - u * u) * Math.sin(phi),
				Math.pow(lambda, 1/3) * u
    ];
	};
*/
	
	private:
};