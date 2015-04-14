#version 120
#extension GL_EXT_geometry_shader4 : enable

#pragma include "../../Common/ShaderHelpers.glslinc"
#pragma include "../../Common/Noise2D.glslinc"

uniform float timeSecs;

uniform vec3 cameraWorldPos;

uniform float stalkHalfWidth;
uniform float stalkHeight;
uniform float stalkSwayingMaxAngle;

uniform float animationTimeMaxDifference;

uniform float grassSwayingNoiseFrequency;
uniform float grassSwayingTimeScale;

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
	
	// Screen-aligned axes, will come in handy for billboarding
	vec3 right		= normalize(vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]));
	vec3 up			= normalize(vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]));
	vec3 forward    = normalize(vec3(gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2]));
	
	float stalkLength = stalkHeight * stalkOrigLength;
	
	// Lets calculate two angles and create a swaying rotation matrix for each stalk
	float swayTime = timeSecs * grassSwayingTimeScale;
	
	// The noise is in the range -1..1
	float swayNoise1 = snoise(  p0.xz                        * grassSwayingNoiseFrequency);
	float swayNoise2 = snoise( (p0.zx + vec2(123.4f,567.8f)) * grassSwayingNoiseFrequency);
	
	float timeOffset1 = swayNoise1 * animationTimeMaxDifference;
	float timeOffset2 = swayNoise2 * animationTimeMaxDifference;

	float swayingAngleRad1 = sin(swayTime + timeOffset1) * stalkSwayingMaxAngle;
	float swayingAngleRad2 = sin(swayTime + timeOffset2) * stalkSwayingMaxAngle;

	mat4 swayingMat = rotationMatrix( vec3(1,0,0), swayingAngleRad1 ) * rotationMatrix( vec3(0,0,1), swayingAngleRad2 );
	
	//color.r = map( sin(swayTime + timeOffset1), -1.0, 1.0, 0.0, 1.0 );
	//color.g = map( cos(swayTime + timeOffset2), -1.0, 1.0, 0.0, 1.0 );
	//color.b = 1.0 - color.g; // Todo: try replacing it with a palette texture.
	
	vec3 stalkSide = right * stalkHalfWidth;
	vec4 stalkVertexPos = p0;
	
	const int vertexOutAmount = 7; // not making this a uniform for now, could be a bit less compatible, non static loop lengths
	float stepMagnitude = stalkLength / float(vertexOutAmount - 1);
	
	for( int i = 0; i < vertexOutAmount; i++ )
	{
		float tmpFrac = float(i) / float(vertexOutAmount-1);
		
		// Let's add a bit of fake light by darkening the stalk towards the bottom
		float darkenAmount = map( smoothstep( 0.4, 1.0, tmpFrac ), 0.0, 1.0,		0.3, 1.0 );
		vec4 darkenColor = vec4( darkenAmount, darkenAmount, darkenAmount, 1.0 );
		
		vec3 tmpStalkHalfSide = stalkSide * (1.0-tmpFrac);
		vec3 tmpStalkNormal = cross( normalize(tmpStalkHalfSide), normalize(stalkUp.xyz) );
		
		gl_Position = gl_ModelViewProjectionMatrix * vec4((stalkVertexPos.xyz - tmpStalkHalfSide), 1.0);
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * vec4((stalkVertexPos.xyz + tmpStalkHalfSide), 1.0);
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		stalkUp = stalkUp * swayingMat;
		stalkVertexPos += stalkUp * stepMagnitude;
	}
}
