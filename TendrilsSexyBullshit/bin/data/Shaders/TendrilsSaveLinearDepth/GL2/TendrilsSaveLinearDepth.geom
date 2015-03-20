
#version 120
#extension GL_EXT_geometry_shader4 : enable

#pragma include "../../Common/ShaderHelpers.glslinc"
#pragma include "../../Common/Noise2D.glslinc"
#pragma include "../../Common/Noise3D.glslinc"
#pragma include "../../Common/Noise4D.glslinc"

uniform float timeSecs;

uniform float stalkRadius;
uniform float stalkHeight;
uniform float stalkSwayingMaxAngle;

uniform float animationTimeMaxDifference;

uniform float grassSwayingNoiseFrequency;
uniform float grassSwayingTimeScale;


varying vec4 out_vertEyeSpace;


#define NUM_RINGS		(6)
#define RING_RESOLUTION (6)

//-------------------------------------------------------------------------------------------------------------------------------------
//
struct RingVertices
{
	vec4 vertexEye[RING_RESOLUTION];
	vec4 vertexEyeProjection[RING_RESOLUTION];
};


//-------------------------------------------------------------------------------------------------------------------------------------
//
void outputVertex( vec4 _vertexEyeSpace, vec4 _vertexEyeProjectionSpace )
{
	out_vertEyeSpace = _vertexEyeSpace;
	gl_Position = _vertexEyeProjectionSpace;
	EmitVertex();
}


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
	
	float newStalkHeight = stalkHeight * stalkOrigLength;
	
	// Lets calculate two angles and create a swaying rotation matrix for each stalk
	float swayTime = timeSecs * grassSwayingTimeScale;
	
	// Offset a time value by a noise calculated from vertex position, then use time with sin to get an angle
	float swayNoise1 = snoise(  p0.xz								 * grassSwayingNoiseFrequency );
	float swayNoise2 = snoise( (0.95 * p0.xz + vec2(123.4f,567.8f)) * grassSwayingNoiseFrequency );
	
	float swayTime1 = swayTime + (swayNoise1 * animationTimeMaxDifference);
	float swayTime2 = swayTime + (swayNoise2 * animationTimeMaxDifference);

	float swayingAngleRad1 = sin(swayTime1) * stalkSwayingMaxAngle;
	float swayingAngleRad2 = sin(swayTime2) * stalkSwayingMaxAngle; 

	// Another way to animate
	//float swayingAngleRad1 = snoise( vec2(p0.x * grassSwayingNoiseFrequency, swayTime) ) * stalkSwayingMaxAngle;
	//float swayingAngleRad2 = snoise( vec2(p0.z * grassSwayingNoiseFrequency, swayTime) ) * stalkSwayingMaxAngle;
	
	mat4 swayingMat = rotationMatrix( vec3(1,0,0), swayingAngleRad1 ) * rotationMatrix( vec3(0,0,1), swayingAngleRad2 );
	//mat4 swayingMat = rotationMatrix( vec3(0,0,1), swayingAngleRad1 ) * rotationMatrix( vec3(1,0,0), swayingAngleRad2 );
	
	vec4 stalkMiddle = p0;
	
	RingVertices rings[NUM_RINGS];
	
	float stepUpWards = newStalkHeight / float(NUM_RINGS - 1);
	
	mat4 stalkPointAt = makeLookAt( vec3(0,0,0), stalkUp.xyz, vec3(0,1,0) );
	mat4 stalkMat = stalkPointAt;
	
	
	// Calculate vertex positions for each ring
	for( int i = 0; i < NUM_RINGS; i++ )
	{
		float tmpFrac = float(i) / float(NUM_RINGS-1);
		
		// By not letting stalkSide go down to 0, we have a difference between the stalkMiddle and the ring vertex, so we can get a normal from it
		vec4 stalkSide = vec4( stalkRadius * (1.0-tmpFrac), 0, 0.001, 1 );
		
		float angStep = TWO_PI / (float(RING_RESOLUTION)-1.0);
		float ang = 0.0;
		
		for ( int j = 0; j < RING_RESOLUTION; j++ )
		{
			mat4 rotAroundRingMat = rotationMatrix( stalkUp.xyz, ang );
			
			vec4 tmpRingVertex = stalkMiddle + ((rotAroundRingMat * stalkMat ) * stalkSide);
			rings[i].vertexEye[j]			= gl_ModelViewMatrix * tmpRingVertex;
			rings[i].vertexEyeProjection[j] = gl_ModelViewProjectionMatrix * tmpRingVertex;
			
			ang += angStep;
		}
		
		stalkMat = stalkMat * swayingMat;		// keep rotating the stalkMat matrix
		stalkUp = stalkUp * swayingMat;			// keep rotating the upwards direction
		stalkMiddle += stalkUp * stepUpWards;	// Move the stalk middle point upwards
	}
	
	// Connect the vertices with triangles
	for( int i = 0; i < NUM_RINGS - 1; i++ )
	{
		float tmpRingFrac = float(i) / float(NUM_RINGS-1);
		
		int ringIndexBot = i;
		int ringIndexTop = i + 1;
		
		for ( int j = 0; j < RING_RESOLUTION - 1; j++ )
		{
			int vertexIndexLeft	 = j;
			int vertexIndexRight = j + 1;
			
			// Eye space
			vec4 topLeftEye  = rings[ringIndexTop].vertexEye[vertexIndexLeft];
			vec4 topRightEye = rings[ringIndexTop].vertexEye[vertexIndexRight];
			
			vec4 botLeftEye  = rings[ringIndexBot].vertexEye[vertexIndexLeft];
			vec4 botRightEye = rings[ringIndexBot].vertexEye[vertexIndexRight];
			
			// Eye Projection space
			vec4 topLeftEyeProj  = rings[ringIndexTop].vertexEyeProjection[vertexIndexLeft];
			vec4 topRightEyeProj = rings[ringIndexTop].vertexEyeProjection[vertexIndexRight];
			
			vec4 botLeftEyeProj  = rings[ringIndexBot].vertexEyeProjection[vertexIndexLeft];
			vec4 botRightEyeProj = rings[ringIndexBot].vertexEyeProjection[vertexIndexRight];
			
			// Start Triangle 1
			
				outputVertex( topLeftEye, topLeftEyeProj );
				outputVertex( topRightEye, topRightEyeProj );
				outputVertex( botRightEye, botRightEyeProj );
			
			EndPrimitive();
			
			
			// Start Triangle 2
			
				outputVertex( topLeftEye, topLeftEyeProj );
				outputVertex( botRightEye, botRightEyeProj );
				outputVertex( botLeftEye, botLeftEyeProj );
			
			EndPrimitive();
			
		}
	}
}
