#version 120

#extension GL_ARB_draw_buffers : enable

// Hmm, do we really need to give the path to the shader if it's in the same folder?
#pragma include "Shaders/Common/ShaderHelpers.glslinc"
#pragma include "Shaders/Common/SimplexNoiseDerivatives4D.glslinc"

uniform sampler2D u_particlePosAndAgeTexture;
uniform sampler2D u_particleVelTexture;
uniform sampler2D u_spawnPosTexture;
uniform sampler2D u_opticalFlowMap;

uniform mat4 u_worldToKinect;
uniform float u_fXToZ;
uniform float u_fYToZ;

uniform float u_maxVel;

uniform float u_flowMagnitude;

uniform vec2 u_pixelDepthmap;
uniform float u_maxFlowMag;

uniform vec2 u_averageFlow;
uniform float u_averageMagnitude;

uniform float u_time;
uniform float u_timeStep;

uniform float u_particleMaxAge;

uniform float u_noisePositionScale = 1.5; // some start values in case we don't set any
uniform float u_noiseMagnitude = 0.075;
uniform float u_noiseTimeScale = 1.0 / 4000.0;
uniform float u_noisePersistence = 0.2;
uniform vec3 u_wind = vec3( 0.5, 0.0, 0.0 );

uniform float u_oldVelToUse = 0.0;

const int OCTAVES = 3;


// -----------------------------------------------------------
vec2 getFlowVelClamped( vec2 _uv, float _maxMag )
{
	vec2 flow = texture2D( u_opticalFlowMap, _uv).xy;
	float flowLength = length(flow);
	
	if( flowLength > _maxMag )
	{
		flow = (flow/flowLength) * _maxMag;
	}
	
	return flow;
}

// -----------------------------------------------------------
vec2 getFlowVelAverage( vec2 _uv, float _maxMag )
{
	vec2 pixel = u_pixelDepthmap;
	vec2 flow = vec2(0.0,0.0);
	flow += getFlowVelClamped( _uv						 , _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  pixel.x,  0.0 ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2( -pixel.x,  0.0 ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  0.0,  pixel.y ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  0.0, -pixel.y ), _maxMag );
	flow /= 5.0;
	
	//flow = texture2D( u_opticalFlowMap, _uv).xy;
	
	return flow;
}

// -----------------------------------------------------------
vec2 getFlowVel( vec3 _worldPos )
{
	vec3 p = (u_worldToKinect * vec4(_worldPos, 1.0)).xyz; // Kinect space pos
	
	float fCoeffX = 1.0 / u_fXToZ;
	float fCoeffY = 1.0 / u_fYToZ;
	
	float nHalfXres = 0.5;
	float nHalfYres = 0.5;
	
	vec2 uvPos;
	uvPos.x = fCoeffX * p.x / p.z + nHalfXres;
	uvPos.y = nHalfYres - fCoeffY * p.y / p.z;
	uvPos.y = 1.0 - uvPos.y;
	
	vec2 flow = getFlowVelAverage( uvPos, u_maxFlowMag );
	flow.y *= -1.0;
	
	return flow;
}

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 posAndAge = texture2D( u_particlePosAndAgeTexture, texCoord );
	vec3 oldVel = texture2D( u_particleVelTexture, texCoord ).xyz;
	vec3 spawnPos  = texture2D( u_spawnPosTexture,			texCoord ).xyz;
	
	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;
	
	age += u_timeStep;
	
	if( age > u_particleMaxAge )
	{
		age = 0.0;
		
		pos = spawnPos;
		
		//float spawnRadius = 0.1;
		//pos = randomPointOnSphere( vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ))) * spawnRadius;
	}
	
	vec3 noisePosition = pos  * u_noisePositionScale;
	float noiseTime    = u_time * u_noiseTimeScale;
	
	/*
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
	
	// Compute curl
	vec3 noiseVel = vec3( zNoisePotentialDerivatives[1] - yNoisePotentialDerivatives[2],
						  xNoisePotentialDerivatives[2] - zNoisePotentialDerivatives[0],
						  yNoisePotentialDerivatives[0] - xNoisePotentialDerivatives[1] ) * u_noiseMagnitude;
	
*/
	
	vec3 newVel = vec3(0.0,0.0,0.0);
	
	newVel    += u_wind;
	newVel    += curlNoise( noisePosition, noiseTime, OCTAVES, u_noisePersistence );
	newVel.xy += getFlowVel( pos ) * u_flowMagnitude;
	newVel.xy += (u_averageFlow * vec2(1.0,-1.0)) * u_averageMagnitude;
	
	float velMag = length(newVel);
	
	newVel = mix( newVel, (newVel/velMag) * u_maxVel, step( u_maxVel, velMag ) ); // limit the length of the velocity
	newVel = mix( newVel, oldVel, u_oldVelToUse );
	
	vec3 newPos = pos + (newVel * u_timeStep);
	
	gl_FragData[0] = vec4( newPos, age );
	gl_FragData[1] = vec4( newVel, 1.0 );
}