
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float timeSecs;

uniform vec3 cameraWorldPos;

uniform float stalkRadius;
uniform float stalkHeight;
uniform float stalkSwayingMaxAngle;

uniform float animationTimeMaxDifference;

uniform float grassSwayingNoiseFrequency;
uniform float grassSwayingTimeScale;

#define HALF_PI  (1.57079632679489661923)
#define PI       (3.14159265358979323846)
#define TWO_PI   (6.28318530717958647693)

//-------------------------------------------------------------------------------------------------------------------------------------
//
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
float map( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }
float mapClamped( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin),    outputMin, outputMax ); }
float stepInOut( float _edge1, float _edge2, float _val ) { return step(_edge1, _val) - step(_edge2,_val); }
float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return smoothstep( _low0, _high0, _t ) * (1.0 - smoothstep( _high1, _low1, _t )); }
vec3  mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2  mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3  permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

//-------------------------------------------------------------------------------------------------------------------------------------
//
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

//-------------------------------------------------------------------------------------------------------------------------------------
//
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

//-------------------------------------------------------------------------------------------------------------------------------------
//
float snoise(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	
	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	
	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
					 + i.x + vec3(0.0, i1.x, 1.0 ));
	
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	
	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

#define NUM_RINGS		(7)
#define RING_RESOLUTION (8)

struct RingVertices
{
	vec4 vertices[RING_RESOLUTION];
	vec4 normals[RING_RESOLUTION];
};

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main()
{
	vec4 color = gl_FrontColorIn[0];
	
	// Let's get the vertices of the line, calculate our stalk up and save how long it was when it came in
	vec4 p0 = gl_PositionIn[0];
	vec4 p1 = gl_PositionIn[1];
	vec4 stalkUp = p1 - p0;
	float stalkOrigLength = length(stalkUp);
	stalkUp = stalkUp / stalkOrigLength;
	
	// Screen-aligned axes, could come in handy
	//vec3 right		= normalize(vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]));
	//vec3 up			= normalize(vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]));
	//vec3 forward    = normalize(vec3(gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2]));
	
	float stalkLength = stalkHeight * stalkOrigLength;
	
	// Lets calculate two angles and create a swaying rotation matrix for each stalk
	float swayTime = timeSecs * grassSwayingTimeScale;
	
	// The noise is in the range -1..1
	float swayNoise1 = snoise(  p0.xz                        * grassSwayingNoiseFrequency);
	float swayNoise2 = snoise( (p0.xz + vec2(123.4f,567.8f)) * grassSwayingNoiseFrequency);
	
	float timeOffset1 = swayNoise1 * animationTimeMaxDifference;
	float timeOffset2 = swayNoise2 * animationTimeMaxDifference;

	float swayingAngleRad1 = sin(swayTime + timeOffset1) * stalkSwayingMaxAngle;
	float swayingAngleRad2 = sin(swayTime + timeOffset2) * stalkSwayingMaxAngle;

	mat4 swayingMat = rotationMatrix( vec3(1,0,0), swayingAngleRad1 ) * rotationMatrix( vec3(0,0,1), swayingAngleRad2 );
	//mat4 swayingMat = rotationMatrix( vec3(0,0,1), swayingAngleRad1 ) * rotationMatrix( vec3(1,0,0), swayingAngleRad2 );
	
	color.r = map( sin(swayTime + timeOffset1), -1.0, 1.0, 0.0, 1.0 );
	color.g = map( cos(swayTime + timeOffset2), -1.0, 1.0, 0.0, 1.0 );
	color.b = 1.0 - color.g; // this could look incredible, try replacing it with a palette texture.
	
	vec4 stalkVertexPos = p0; // + vec4( (right*tmpRand*randomMovementMagnitude) + (up*tmpRand*randomMovementMagnitude), 1.0);
	
	RingVertices rings[NUM_RINGS];
	vec4 ringColors[NUM_RINGS];
	
	float stepUpWards = stalkLength / float(NUM_RINGS - 1);
	
	mat4 stalkPointAt = makeLookAt( vec3(0,0,0), stalkUp.xyz, vec3(0,1,0) );
	mat4 stalkMat = stalkPointAt;
	
	for( int i = 0; i < NUM_RINGS; i++ )
	{
		float tmpFrac = float(i) / float(NUM_RINGS-1);
		
		// Let's add a bit of fake light by darkening the stalk towards the bottom
		float darkenAmount = map( smoothstep( 0.4, 1.0, tmpFrac ), 0.0, 1.0,		0.3, 1.0 );
	
		ringColors[i] = color * vec4( darkenAmount, darkenAmount, darkenAmount, 1.0 );
		
		vec4 stalkSide = vec4( stalkRadius * (1.0-tmpFrac), 0, 0, 1 );
		
		vec4 stalkMiddle = stalkVertexPos + (stalkUp * stepUpWards);
		
		float angStep = TWO_PI / (float(RING_RESOLUTION)-1.0);
		float ang = 0.0;
		
		for ( int j = 0; j < RING_RESOLUTION; j++ )
		{
			mat4 rotAroundRingMat = rotationMatrix( stalkUp.xyz, ang );
			
			vec4 posAroundRing = stalkMiddle + ((stalkMat * rotAroundRingMat) * stalkSide);
			rings[i].vertices[j] = posAroundRing;
			rings[i].normals[j] = normalize( posAroundRing - stalkMiddle);
			
			ang += angStep;
		}
		
		// TODO: Use length we passed in via the normal to vary stalk lengths
		
		stalkMat = stalkMat * swayingMat; // keep rotating the matrix
		stalkUp = stalkUp * swayingMat; // keep rotating the upwards normal
		stalkVertexPos += stalkUp * stepUpWards;
	}
	
	for( int i = 0; i < NUM_RINGS - 1; i++ )
	{
		float tmpRingFrac = float(i) / float(NUM_RINGS-1);
		
		// Let's add a bit of fake light by darkening the stalk towards the bottom
		float darken = map( smoothstep( 0.4, 1.0, tmpRingFrac ), 0.0, 1.0,		0.3, 1.0 );
		vec4 ringColor = color * vec4( darken, darken, darken, 1.0 ); // It's faster to compute this here than reading from an array
		
		int ringIndexBot = i;
		int ringIndexTop = i + 1;
		
		for ( int j = 0; j < RING_RESOLUTION - 1; j++ )
		{
			int vertexIndex		= j;
			int vertexIndexNext = j + 1;
			
			/*
			// Points
			gl_Position = gl_ModelViewProjectionMatrix * rings[i].vertices[j];
			gl_FrontColor = ringColor;
			EmitVertex();
			*/
			
			/*
			// Lines along normal (NOT WORKING)
			gl_Position = gl_ModelViewProjectionMatrix * rings[i].vertices[j];
			gl_FrontColor = ringColor;
			EmitVertex();
			
			gl_Position = gl_ModelViewProjectionMatrix * (rings[i].vertices[j] + rings[i].normals[j] * 0.2);
			//gl_Position = gl_ModelViewProjectionMatrix * rings[i].vertices[j];
			gl_FrontColor = ringColor;
			EmitVertex();
			
			EndPrimitive();
			 */
			
			vec4 topLeft  = gl_ModelViewProjectionMatrix * rings[ringIndexTop].vertices[vertexIndex];
			vec4 topRight = gl_ModelViewProjectionMatrix * rings[ringIndexTop].vertices[vertexIndexNext];
			
			vec4 botLeft  = gl_ModelViewProjectionMatrix * rings[ringIndexBot].vertices[vertexIndex];
			vec4 botRight = gl_ModelViewProjectionMatrix * rings[ringIndexBot].vertices[vertexIndexNext];
			
			vec4 colorTop = ringColors[ringIndexTop];
			vec4 colorBot = ringColors[ringIndexBot];

			
			// Triangle 1
			gl_Position = topLeft;
			gl_FrontColor = colorTop;
			EmitVertex();
		
			gl_Position = topRight;
			gl_FrontColor = colorTop;
			EmitVertex();
			
			gl_Position = botRight;
			gl_FrontColor = colorBot;
			EmitVertex();
			
			EndPrimitive();
			
			
			// Triangle 2
			gl_Position = topLeft;
			gl_FrontColor = colorTop;
			EmitVertex();
			
			gl_Position = botRight;
			gl_FrontColor = colorBot;
			EmitVertex();
			
			gl_Position = botLeft;
			gl_FrontColor = colorBot;
			EmitVertex();
			
			EndPrimitive();
			
		}
	}
}
