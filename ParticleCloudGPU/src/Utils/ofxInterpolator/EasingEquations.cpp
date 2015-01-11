
#include "EasingEquations.h"


EasingEquations::EaseType EasingEquations::nextEaseType( EaseType _type )
{
	EasingEquations::EaseType tmp = EaseType( _type + 1);
	if( tmp >= EASE_MODE_AMOUNT) tmp = EASE_MODE_FIRST;
	return tmp;
}

EasingEquations::EaseType EasingEquations::prevEaseType( EaseType _type )
{
	EasingEquations::EaseType tmp = EaseType( _type - 1);
	if( tmp < EASE_MODE_FIRST) tmp = EASE_MODE_LAST;
	return tmp;
}


float EasingEquations::getValueAtFrac( float _startVal, float _targetVal, float _frac )
{
	return _startVal + ( ( _targetVal-_startVal) * _frac );
}

float EasingEquations::ease( float t, EaseType easeType )
{
	switch (easeType)
	{
		case EASE_LINEAR:
			return linearTween( t, 0.0f, 1.0f );
			break;

		case EASE_IN_QUAD:
			return easeInQuad( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_QUAD:
			return easeOutQuad( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_QUAD:
			return easeInOutQuad( t, 0.0f, 1.0f );
			break;


		case EASE_IN_CUBIC:
			return easeInCubic( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_CUBIC:
			return easeOutCubic( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_CUBIC:
			return easeInOutCubic( t, 0.0f, 1.0f );
			break;


		case EASE_IN_QUART:
			return easeInQuart( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_QUART:
			return easeOutQuart( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_QUART:
			return easeInOutQuart( t, 0.0f, 1.0f );
			break;
	

		case EASE_IN_QUINT:
			return easeInQuint( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_QUINT:
			return easeOutQuint( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_QUINT:
			return easeInOutQuint( t, 0.0f, 1.0f );
			break;

		case EASE_IN_SINE:
			return easeInSine( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_SINE:
			return easeOutSine( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_SINE:
			return easeInOutSine( t, 0.0f, 1.0f );
			break;


		case EASE_IN_EXPO:
			return easeInExpo( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_EXPO:
			return easeOutExpo( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_EXPO:
			return easeInOutExpo( t, 0.0f, 1.0f );
			break;

		case EASE_IN_CIRCULAR:
			return easeInCircular( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_CIRCULAR:
			return easeOutCircular( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_CIRCULAR:
			return easeInOutCircular( t, 0.0f, 1.0f );
			break;


		case EASE_IN_ELASTIC:
			return easeInElastic( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_ELASTIC:
			return easeOutElastic( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_ELASTIC:
			return easeInOutElastic( t, 0.0f, 1.0f );
			break;


		case EASE_IN_BACK:
			return easeInBack( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_BACK:
			return easeOutBack( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_BACK:
			return easeInOutBack( t, 0.0f, 1.0f );
			break;

		case EASE_IN_BOUNCE:
			return easeInBounce( t, 0.0f, 1.0f );
			break;
		case EASE_OUT_BOUNCE:
			return easeOutBounce( t, 0.0f, 1.0f );
			break;
		case EASE_INOUT_BOUNCE:
			return easeInOutBounce( t, 0.0f, 1.0f );
			break;


		default:
			return easeInOutQuad( t, 0.0f, 1.0f );
			break;
	}
}





// simple linear tweening - no easing
// t: current time, b: beginning value, c: change in value, d: duration
float EasingEquations::linearTween( float t, float b, float c, float d) 
{
	return c*t/d + b;
};



float EasingEquations::easeInQuad( float t, float b, float c, float d  )
{
	t/=d;
	return c*t*t + b;
}

float EasingEquations::easeOutQuad( float t, float b, float c, float d  )
{
	t/=d;
	return -c *(t)*(t-2.0f) + b;
}

float EasingEquations::easeInOutQuad( float t, float b, float c, float d )
{
	t /= d/2.0f;
	if (t < 1.0f) { return c/2.0f*t*t + b; }
	else 
	{
		t -= 1.0f;
		return -c/2.0f * (t*(t-2.0f) - 1.0f) + b;
	}
}

float EasingEquations::easeInCubic( float t, float b, float c, float d  )
{
	t/=d;
	return c*(t)*t*t + b;
}

float EasingEquations::easeOutCubic( float t, float b, float c, float d  )
{
	t = t/d - 1.0f;
	return c*((t)*t*t + 1.0f) + b;
}

float EasingEquations::easeInOutCubic( float t, float b, float c, float d )
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t*t + b;
	t-=2.0f;
	return c/2.0f*((t)*t*t + 2.0f) + b;
}


float EasingEquations::easeInQuart( float t, float b, float c, float d )
{
	t/=d;
	return c*(t)*t*t*t + b;
}

float EasingEquations::easeOutQuart( float t, float b, float c, float d )
{
	t=t/d-1;
	return -c * ((t)*t*t*t - 1.0f) + b;
}

float EasingEquations::easeInOutQuart( float t, float b, float c, float d )
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2.0f*t*t*t*t + b;
	t-=2.0f;
	return -c/2.0f * (t*t*t*t - 2.0f) + b;
}


float EasingEquations::easeInQuint( float t, float b, float c, float d )
{
	t/=d;
	return c*t*t*t*t*t + b;
}

float EasingEquations::easeOutQuint( float t, float b, float c, float d )
{
	t=t/d-1.0f;
	return c*(t*t*t*t*t + 1.0f) + b;
}

float EasingEquations::easeInOutQuint( float t, float b, float c, float d )
{
	t /= d/2.0f;
	if (t < 1.0f) return c/2*t*t*t*t*t + b;
	t-=2.0f;
	return c/2.0f*((t)*t*t*t*t + 2.0f) + b;
}


float EasingEquations::easeInSine( float t, float b, float c, float d )
{
	return -c * cosf(t/d * EASE_PI_DIV2) + c + b;
}

float EasingEquations::easeOutSine( float t, float b, float c, float d )
{
	return c * sinf(t/d * EASE_PI_DIV2) + b;
}

float EasingEquations::easeInOutSine( float t, float b, float c, float d  )
{
	return -c/2.0f * (cosf(EASE_PI*t/d) - 1.0f) + b;
}



float EasingEquations::easeInExpo( float t, float b, float c, float d )
{
	return (t==0.0f) ? b : c * pow(2.0f, 10.0f * (t/d - 1.0f)) + b;
}

float EasingEquations::easeOutExpo( float t, float b, float c, float d )
{
	return (t==d) ? b+c : c * (-pow(2.0f, -10.0f * t/d) + 1.0f) + b;
}

float EasingEquations::easeInOutExpo( float t, float b, float c, float d )
{
	if (t==0.0f) return b;
	if (t==d) return b+c;
	t /= d/2.0f;
	if (t < 1.0f) return c/2 * pow(2.0f, 10.0f * (t - 1.0f)) + b;
	return c/2.0f * (-pow(2.0f, -10.0f * --t) + 2.0f) + b;
}

float EasingEquations::easeInCircular( float t, float b, float c, float d )
{
	t/=d;
	return -c * ( sqrt(1.0f - t*t) - 1.0f) + b;
}

float EasingEquations::easeOutCircular( float t, float b, float c, float d )
{
	t=t/d-1.0f;
	return c * sqrt(1.0f - t*t) + b;
}

float EasingEquations::easeInOutCircular( float t, float b, float c, float d)
{
	t /= d/2.0f;
	if (t < 1.0f) return -c/2.0f * (sqrt(1.0f - t*t) - 1.0f) + b;
	t-=2.0f;
	return c/2.0f * (sqrt(1.0f - t*t) + 1.0f) + b;
}


/////////// ELASTIC EASING: exponentially decaying sine wave  //////////////

// t: current time, b: beginning value, c: change in value, d: duration, a: amplitude (optional), p: period (optional)
// t and d can be in frames or seconds/milliseconds
float EasingEquations::easeInElastic( float t, float b, float c, float d , float a, float p )
{
	if (t == 0.0f) return b; 
	t /= d;
	if (t==1.0f) return b+c;  
	
	float s = 0.0f;

	if (a < fabs(c)) { a = c; s = p/4.0f; }
	else s = p/(2.0f*EASE_PI) * asin (c/a);

	t -= 1.0f;
	return -(a*pow(2.0f,10.0f*t) * sin( (t*d-s)*(EASE_TWOPI)/p )) + b;
}

float EasingEquations::easeOutElastic( float t, float b, float c, float d , float a, float p )
{
	if (t==0) return b;  
	t /= d;
	if (t==1.0f) return b+c;  

	float s = 0.0f;

	if (a < fabs(c)) { a=c; s=p/4.0f; }
	else s = p/(2.0f*EASE_PI) * asin (c/a);

	return a*pow(2.0f,-10.0f*t) * sin( (t*d-s)*(EASE_TWOPI)/p ) + c + b;
}

float EasingEquations::easeInOutElastic( float t, float b, float c, float d , float a, float p )
{
	if (t==0) return b;  
	if ((t/=d/2)==2) return b+c; 

	float s = 0.0f;

	if (a < fabs(c)) { a=c; s=p/4.0f; }

	else s = p/EASE_TWOPI * asin (c/a);

	t-=1.0f;
	if (t < 1.0f) return -0.5f*(a*pow(2.0f,10.0f*(t)) * sin( (t*d-s)*(EASE_TWOPI)/p )) + b;

	t-=1.0f;
	return a*pow(2.0f,-10.0f*(t)) * sin( (t*d-s)*(EASE_TWOPI)/p )*0.5f + c + b;
}


/////////// BACK EASING: overshooting cubic easing: (s+1)*t^3 - s*t^2  //////////////
// back easing in - backtracking slightly, then reversing direction and moving to target
// t: current time, b: beginning value, c: change in value, d: duration, s: overshoot amount (optional)
// t and d can be in frames or seconds/milliseconds
// s controls the amount of overshoot: higher s means greater overshoot
// s has a default value of 1.70158, which produces an overshoot of 10 percent
// s==0 produces cubic easing with no overshoot
float EasingEquations::easeInBack( float t, float b, float c, float d, float s )
{
	t/=d;
	return c*(t)*t*((s+1.0f)*t - s) + b;
}

float EasingEquations::easeOutBack( float t, float b, float c, float d, float s )
{
	t=t/d-1.0f;
	return c*((t)*t*((s+1.0f)*t + s) + 1.0f) + b;
}
float EasingEquations::easeInOutBack( float t, float b, float c, float d, float s )
{
	t /= d/2.0f;
	s*=(1.525f);
	if (t < 1) return c/2.0f*(t*t*(((s)+1.0f)*t - s)) + b;
	t-=2.0f;
	s*=(1.525f);
	return c/2*((t)*t*(((s)+1.0f)*t + s) + 2.0f) + b;
}


/////////// BOUNCE EASING: exponentially decaying parabolic bounce  //////////////
// bounce easing in
// t: current time, b: beginning value, c: change in position, d: duration
float EasingEquations::easeInBounce( float t, float b, float c, float d )
{
	return c - easeOutBounce (d-t, 0.0f, c, d) + b;
}

float EasingEquations::easeOutBounce( float t, float b, float c, float d )
{
	if ((t/=d) < (1.0f/2.75f)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < (2.0f/2.75f)) {
		t-=(1.5f/2.75f);
		return c*(7.5625f*(t)*t + .75f) + b;
	} else if (t < (2.5f/2.75f)) {
		t-=(2.25f/2.75f);
		return c*(7.5625f*(t)*t + .9375f) + b;
	} else {
		t-=(2.625f/2.75f);
		return c*(7.5625f*(t)*t + .984375f) + b;
	}
}

float EasingEquations::easeInOutBounce( float t, float b, float c, float d )
{
	if (t < d/2.0f) return easeInBounce (t*2.0f, 0.0f, c, d) * 0.5f + b;
	return easeOutBounce (t*2.0f-d, 0.0f, c, d) * 0.5f + c*0.5f + b;
}


/*


/////////// BOUNCE EASING: exponentially decaying parabolic bounce  //////////////
// bounce easing in
// t: current time, b: beginning value, c: change in position, d: duration
Math.easeInBounce = function (t, b, c, d) {
return c - Math.easeOutBounce (d-t, 0, c, d) + b;
};

// bounce easing out
Math.easeOutBounce = function (t, b, c, d) {
if ((t/=d) < (1/2.75)) {
return c*(7.5625*t*t) + b;
} else if (t < (2/2.75)) {
return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
} else if (t < (2.5/2.75)) {
return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
} else {
return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
}
};

// bounce easing in/out
Math.easeInOutBounce = function (t, b, c, d) {
if (t < d/2) return Math.easeInBounce (t*2, 0, c, d) * .5 + b;
return Math.easeOutBounce (t*2-d, 0, c, d) * .5 + c*.5 + b;
};

*/
