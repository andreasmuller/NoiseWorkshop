
#version 120
#extension GL_ARB_draw_buffers : enable

#pragma include "Shaders/Common/ShaderHelpers.glslinc"
#pragma include "Shaders/Common/Noise4D.glslinc"
//#pragma include "Shaders/Common/SimplexNoiseDerivatives4D.glslinc"


uniform sampler2D u_positionAndAgeTex;

uniform float u_time;
uniform float u_timeStep;

uniform float u_particleMaxAge;

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
		
		float spawnRadius = 1.0;
		pos = randomPointOnSphere( vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ))) * spawnRadius;
	}
	
	vec3 vel = vec3(0,2.5,0);

	vec3 noisePos = pos * 0.8;
	//vel += normalize(getNoiseOffset(vec4(noisePos, u_time * 0.4))) * 1.0;
	//vel += curlNoise( noisePos, u_time * 0.6, 3, 0.5 ) * 0.9;
	vel += fbmvec3( vec4(noisePos, u_time * 0.5), 8, 2.0, 0.2 ) * 2.0;

	vec3 newPos = pos + vel * u_timeStep;
	pos = newPos;
	
	gl_FragData[0] = vec4( pos, age );
}

