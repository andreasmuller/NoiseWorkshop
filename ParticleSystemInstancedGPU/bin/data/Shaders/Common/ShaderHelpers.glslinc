#define HALF_PI  1.57079632679489661923
#define PI       3.14159265358979323846
#define TWO_PI   6.28318530717958647693

float map( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }
float mapClamped( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin),    outputMin, outputMax ); }

vec3  map( vec3 value, vec3 inputMin, vec3 inputMax, vec3 outputMin, vec3 outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }

float stepInOut( float _edge1, float _edge2, float _val ) { return step(_edge1, _val) - step(_edge2,_val); }

float linearStep( float _edge0, float _edge1, float _t ) { return clamp( (_t - _edge0)/(_edge1 - _edge0), 0.0, 1.0); }
float linearStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return linearStep( _low0, _high0, _t ) * (1.0f - linearStep( _high1, _low1, _t )); }

float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return smoothstep( _low0, _high0, _t ) * (1.0 - smoothstep( _high1, _low1, _t )); }

float mod289(float x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x)   { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 mod289(vec3 x)   { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x)   { return x - floor(x * (1.0 / 289.0)) * 289.0; }

float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }

float permute(float x) { return mod289(((x*34.0)+1.0)*x); }
vec3 permute(vec3 x)   { return mod289(((x*34.0)+1.0)*x); }
vec4 permute(vec4 x)   { return mod289(((x*34.0)+1.0)*x); }

float taylorInvSqrt(float r) { return 1.79284291400159 - 0.85373472095314 * r; }
vec4 taylorInvSqrt(vec4 r)   { return 1.79284291400159 - 0.85373472095314 * r; }


// ------------------------------------------------------------
mat4 makeLookAt(vec3 eye, vec3 center, vec3 up)
{
	mat4 M;
	
	vec3 zaxis = normalize(eye - center);
	vec3 xaxis = normalize( cross(up, zaxis) );
	vec3 yaxis = cross(zaxis,xaxis);
	
	M[0] = vec4(xaxis,0);
	M[1] = vec4(yaxis,0);
	M[2] = vec4(zaxis,0);
	M[3] = vec4(eye,1);
	
	return M;
}

// ------------------------------------------------------------
mat4 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
				oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
				oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
				0.0,                                0.0,                                0.0,                                1.0);
}


// --------------------------------------
vec3 randomPointOnSphere( vec3 _random )
{
	float lambda = _random.x;
	float u = map( _random.y, 0.0, 1.0, -1.0, 1.0 );
	float phi = _random.z * (2.0 * PI );
	
	vec3 p;
	p.x = pow(lambda, 1.0/3.0) * sqrt(1.0 - u * u) * cos(phi);
	p.y = pow(lambda, 1.0/3.0) * sqrt(1.0 - u * u) * sin(phi);
	p.z = pow(lambda, 1.0/3.0) * u;
	
	return p;
}


// ------------------------------------------------------------
float pulseSquare( float _t, float _frequency, float _width )
{
	return 1.0 - step( _width, mod( _t, _frequency ) );
}

// ------------------------------------------------------------
float pulseTriangle( float _t, float _frequency, float _width )
{
	float triangleT = mod( _t, _frequency ) / _width * 2.0;
	return (1.0 - abs(mod(triangleT,2.0) - 1.0)) * pulseSquare( _t, _frequency, _width );
}

// ------------------------------------------------------------
float pulseLineDownUp( float _t, float _frequency, float _width )
{
	float tmpVal = mod( _t, _frequency ) / _width;
	return tmpVal * (1.0 - step( 1.0, tmpVal ));
}

// ------------------------------------------------------------
float pulseLineUpDown( float _t, float _frequency, float _width )
{
	float tmpVal = 1 - (mod( _t, _frequency ) / _width);
	return clamp( tmpVal * (1 - step( 1.0, tmpVal )), 0, 1);
}

// ------------------------------------------------------------
float pulseSawTooth( float _t, float _frequency, float _width )
{
	float tmpVal = 1 - (mod( _t, _frequency ) / _width);
	return clamp( tmpVal * (1.0 - step( 1.0, tmpVal )), 0.0, 1.0);
}

// ------------------------------------------------------------
float pulseSine( float _t, float _frequency, float _width )
{
	float tmpVal = clamp( (mod( _t, _frequency ) / _width), 0.0, 1.0);
	return sin(tmpVal * PI);
}

// -----------------------------------------------------------
float pulseSmoothStep( float _t, float _frequency, float _x0, float _x1, float _x2, float _x3 )
{
	float tmpVal = mod( _t, _frequency );
	return smoothStepInOut( _x0, _x1, _x2, _x3, tmpVal ) ;
}

// -----------------------------------------------------------
float pulseLinearStep( float _t, float _frequency, float _x0, float _x1, float _x2, float _x3 )
{
	float tmpVal = mod( _t, _frequency );
	return linearStepInOut( _x0, _x1, _x2, _x3, tmpVal ) ;
}

// --------------------------------------
vec3 opTwistX( vec3 _p, float _angPerUnit )
{
	float c = cos( _angPerUnit * _p.x);
	float s = sin( _angPerUnit * _p.x);
	mat2  m = mat2(c,-s,s,c);
	return vec3( _p.x, m * _p.yz );
}

// --------------------------------------
vec3 opTwistY( vec3 _p, float _angPerUnit )
{
	float c = cos( _angPerUnit * _p.y);
	float s = sin( _angPerUnit * _p.y);
	mat2  m = mat2(c,-s,s,c);
	
	vec2 rotXZ = m * _p.xz;
	
	return vec3( rotXZ.x, _p.y, rotXZ.y );
}

// --------------------------------------
vec3 opTwistZ( vec3 _p, float _angPerUnit )
{
	float c = cos( _angPerUnit * _p.z);
	float s = sin( _angPerUnit * _p.z);
	mat2  m = mat2(c,-s,s,c);
	
	vec2 rotXY = m * _p.xy;
	
	return vec3( rotXY.x, rotXY.y, _p.z );
}

// --------------------------------------
float CatmullRom( float u, float x0, float x1, float x2, float x3 )
{
	float u2 = u * u;
	float u3 = u2 * u;
	return ((2 * x1) +
			(-x0 + x2) * u +
			(2*x0 - 5*x1 + 4*x2 - x3) * u2 +
			(-x0 + 3*x1 - 3*x2 + x3) * u3) * 0.5;
}

// http://www.iquilezles.org/www/articles/functions/functions.htm

// --------------------------------------
float cubicPulse( float c, float w, float x )
{
	x = abs(x - c);
	if( x>w ) return 0.0;
	x /= w;
	return 1.0 - x*x*(3.0-2.0*x);
}

// --------------------------------------
float expStep( float x, float k, float n )
{
	return exp( -k*pow(x,n) );
}

// --------------------------------------
float parabola( float x, float k )
{
	return pow( 4.0*x*(1.0-x), k );
}

// --------------------------------------
float pcurve( float x, float a, float b )
{
	float k = pow(a+b,a+b) / (pow(a,a)*pow(b,b));
	return k * pow( x, a ) * pow( 1.0-x, b );
}

// --------------------------------------
float impulse( float k, float x )
{
	float h = k*x;
	return h*exp(1.0-h);
}
