#version 120

#extension GL_ARB_draw_buffers : enable

// Hmm, do we really need to give the path to the shader if it's in the same folder?
#pragma include "Shaders/Common/ShaderHelpers.glslinc"
//#pragma include "Shaders/Common/SimplexNoiseDerivatives4D.glslinc"
#pragma include "Shaders/Common/Noise4D.glslinc"

uniform sampler2D u_positionAndAgeTex;

uniform float u_time;
uniform float u_timeStep;

uniform float u_particleMaxAge;

uniform float u_noisePositionScale = 1.5;
uniform float u_noiseMagnitude = 0.075;
uniform float u_noiseTimeScale = 1.0 / 4000.0;
uniform float u_noisePersistence = 0.2;
uniform vec3 u_wind = vec3( 0.5, 0.0, 0.0 );

const int OCTAVES = 3;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 posAndAge = texture2D( u_positionAndAgeTex, texCoord );
	
	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;
	
	age += u_timeStep;
	
	if( age > u_particleMaxAge )
	{
		age = age - u_particleMaxAge;
		
		float spawnRadius = 0.1;
		pos = randomPointOnSphere( vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ))) * spawnRadius;
	}
	
	vec3 noisePosition = pos  * u_noisePositionScale;
	float noiseTime    = u_time * u_noiseTimeScale;

	vec3 noiseVelocity = fbmvec3( vec4(noisePosition, noiseTime), OCTAVES, 2.0, u_noisePersistence ) * u_noiseMagnitude;
	//vec3 noiseVelocity = curlNoise( noisePosition, noiseTime, OCTAVES, u_noisePersistence ) * u_noiseMagnitude;
	
	vec3 totalVelocity = u_wind + noiseVelocity;
	
	vec3 newPos = pos + totalVelocity * u_timeStep;
	
	pos = newPos;
	
	gl_FragData[0] = vec4( pos, age );	
}

