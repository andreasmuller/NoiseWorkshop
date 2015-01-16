#version 120

#extension GL_ARB_draw_buffers : enable

// Hmm, do we really need to give the path to the shader if it's in the same folder?
#pragma include "Shaders/Common/ShaderHelpers.glslinc"
#pragma include "Shaders/Common/SimplexNoiseDerivatives4D.glslinc"

uniform sampler2D u_particlePosAndAgeTexture;
uniform sampler2D u_particleVelTexture;

uniform float u_time;
uniform float u_timeStep;

uniform float u_particleMaxAge;

uniform float u_noisePositionScale = 1.5; // some start values in case we don't set any
uniform float u_noiseMagnitude = 0.075;
uniform float u_noiseTimeScale = 1.0 / 4000.0;
uniform float u_noisePersistence = 0.2;
uniform vec3 u_baseSpeed = vec3( 0.5, 0.0, 0.0 );

const int OCTAVES = 3;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 posAndAge = texture2D( u_particlePosAndAgeTexture, texCoord );
	
	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;
	
	age += u_timeStep;
	
	if( age > u_particleMaxAge )
	{
		age = 0.0;
		
		float spawnRadius = 0.1;
		pos = randomPointOnSphere( vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ))) * spawnRadius;
	}
	
	vec3 noisePosition = pos  * u_noisePositionScale;
	float noiseTime    = u_time * u_noiseTimeScale;
	
	vec4 xNoisePotentialDerivatives = vec4(0.0);
	vec4 yNoisePotentialDerivatives = vec4(0.0);
	vec4 zNoisePotentialDerivatives = vec4(0.0);
	
	float tmpPersistence = u_noisePersistence;
	
	for (int i = 0; i < OCTAVES; ++i)
	{
		float scale = (1.0 / 2.0) * pow(2.0, float(i));
		
		float noiseScale = pow(tmpPersistence, float(i));
		if (tmpPersistence == 0.0 && i == 0) //fix undefined behaviour
		{
			noiseScale = 1.0;
		}
		
		xNoisePotentialDerivatives += simplexNoiseDerivatives(vec4(noisePosition * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
		yNoisePotentialDerivatives += simplexNoiseDerivatives(vec4((noisePosition + vec3(123.4, 129845.6, -1239.1)) * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
		zNoisePotentialDerivatives += simplexNoiseDerivatives(vec4((noisePosition + vec3(-9519.0, 9051.0, -123.0))  * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
	}
	
	//compute curl
	vec3 noiseVelocity = vec3( zNoisePotentialDerivatives[1] - yNoisePotentialDerivatives[2],
							   xNoisePotentialDerivatives[2] - zNoisePotentialDerivatives[0],
							   yNoisePotentialDerivatives[0] - xNoisePotentialDerivatives[1] ) * u_noiseMagnitude;
	
	vec3 totalVelocity = u_baseSpeed + noiseVelocity;
	
	vec3 newPos = pos + totalVelocity * u_timeStep;
	vec3 vel = newPos - pos;
	
	pos = newPos;
	
	gl_FragData[0] = vec4( pos, age );
	gl_FragData[1] = vec4( vel, 1.0 );
	
}

