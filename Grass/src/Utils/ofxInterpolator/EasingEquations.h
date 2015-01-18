

#pragma once

// Taken largely from Robert Penners easing equations

#define EASE_PI		   3.1415926535897932384626433832795f
#define EASE_TWOPI      6.283185307179586476925287f
#define EASE_PI_DIV2       1.570796326794896619231322f

#include <cmath>



#define EASE_MODE_FIRST		EASE_LINEAR
#define EASE_MODE_LAST		EASE_INOUT_BOUNCE
#define EASE_MODE_AMOUNT	( EASE_MODE_LAST - EASE_MODE_FIRST + 1)

class EasingEquations
{

public:

	enum EaseType
	{
		EASE_LINEAR = 0,
		
		EASE_IN_QUAD,
		EASE_OUT_QUAD,
		EASE_INOUT_QUAD,
		
		EASE_IN_CUBIC,
		EASE_OUT_CUBIC,
		EASE_INOUT_CUBIC,
		
		EASE_IN_QUART,
		EASE_OUT_QUART,
		EASE_INOUT_QUART,
		
		EASE_IN_QUINT,
		EASE_OUT_QUINT,
		EASE_INOUT_QUINT,
		
		EASE_IN_SINE,
		EASE_OUT_SINE,
		EASE_INOUT_SINE,
		
		EASE_IN_EXPO,
		EASE_OUT_EXPO,
		EASE_INOUT_EXPO,
		
		EASE_IN_CIRCULAR,
		EASE_OUT_CIRCULAR,
		EASE_INOUT_CIRCULAR,
		
		EASE_IN_ELASTIC,
		EASE_OUT_ELASTIC,
		EASE_INOUT_ELASTIC,
		
		EASE_IN_BACK,
		EASE_OUT_BACK,
		EASE_INOUT_BACK,
		
		EASE_IN_BOUNCE,
		EASE_OUT_BOUNCE,
		EASE_INOUT_BOUNCE,

		EASE_EQUATION_AMOUNT

	};	
	
	static float getValueAtFrac( float _startVal, float _targetVal, float _frac );

	static float ease( float t, EaseType easeType = EASE_INOUT_QUAD );

	static float linearTween( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f );

	static float easeInQuad( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutQuad( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutQuad( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );

	static float easeInCubic( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutCubic( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutCubic( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );

	static float easeInQuart( float t, float b = 0.0f, float c = 1.0f, float d  = 1.0f );
	static float easeOutQuart( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutQuart( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );


	static float easeInQuint( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutQuint( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutQuint( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );

	static float easeInSine( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutSine( float t, float b = 0.0f, float c = 1.0f, float d  = 1.0f );
	static float easeInOutSine( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );

	static float easeInExpo( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutExpo( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutExpo( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );

	static float easeInCircular( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeOutCircular( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );
	static float easeInOutCircular( float t, float b = 0.0f, float c = 1.0f, float d = 1.0f  );


	/////////// ELASTIC EASING: exponentially decaying sine wave  //////////////
	// t: current time, b: beginning value, c: change in value, d: duration, a: amplitude (optional), p: period (optional)
	// t and d can be in frames or seconds/milliseconds
	static float easeInElastic( float t, float b, float c, float d = 1.0f, float a = 1.0f, float p = 0.3f );
	static float easeOutElastic( float t, float b, float c, float d = 1.0f, float a = 1.0f, float p = 0.3f );
	static float easeInOutElastic( float t, float b, float c, float d = 1.0f, float a = 1.0f, float p = 0.45f );

	/////////// BACK EASING: overshooting cubic easing: (s+1)*t^3 - s*t^2  //////////////
	// back easing in - backtracking slightly, then reversing direction and moving to target
	// t: current time, b: beginning value, c: change in value, d: duration, s: overshoot amount (optional)
	// t and d can be in frames or seconds/milliseconds
	// s controls the amount of overshoot: higher s means greater overshoot
	// s has a default value of 1.70158, which produces an overshoot of 10 percent
	// s==0 produces cubic easing with no overshoot
	static float easeInBack( float t, float b, float c, float d = 1.0f, float s = 1.70158f );
	static float easeOutBack( float t, float b, float c, float d = 1.0f, float s = 1.70158f );
	static float easeInOutBack( float t, float b, float c, float d = 1.0f, float s = 1.70158f );

	/////////// BOUNCE EASING: exponentially decaying parabolic bounce  //////////////
	// bounce easing in
	// t: current time, b: beginning value, c: change in position, d: duration
	static float easeInBounce( float t, float b, float c, float d = 1.0f  );
	static float easeOutBounce( float t, float b, float c, float d = 1.0f  );
	static float easeInOutBounce( float t, float b, float c, float d = 1.0f  );

	static EaseType nextEaseType( EaseType _type );
	static EaseType prevEaseType( EaseType _type );
	
	
};


