#version 120
#extension GL_EXT_gpu_shader4 : require

#pragma include "Shaders/Common/ShaderHelpers.glslinc"

uniform vec2 u_resolution;

uniform float u_time;

uniform sampler2D u_particlePosAndAgeTexture;
uniform sampler2D u_particleVelTexture;

uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjectionMatrix;

uniform float u_particleMaxAge;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

uniform int u_numLights = 0;

varying vec3 v_normal;
varying vec3 v_eyeVec;

#define MAX_LIGHTS 8
varying vec3 v_lightDir[MAX_LIGHTS];

varying vec4 v_particleColor;


// ----------------------------
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

// ----------------------------
void main ()
{
	// Figure out the texture coordinate our data is on from the instance ID
	vec2 texCoord;
	texCoord.x = mod(gl_InstanceID, u_resolution.x) / u_resolution.x;
	texCoord.y = floor(gl_InstanceID / u_resolution.x) / u_resolution.y;
	
	// Grab our data
	vec4 particleData = texture2D( u_particlePosAndAgeTexture, texCoord );
	vec3 particleVel = texture2D( u_particleVelTexture, texCoord ).xyz;

	float ageFrac = particleData.w / u_particleMaxAge;
	vec4 vertexPos = gl_Vertex;
	vec3 particlePos = particleData.xyz;
	
	// Pass the particle color along to the fragment shader
	v_particleColor = mix(u_particleStartColor, u_particleEndColor, ageFrac );
	
	// We are going to scale the particle so it comes in and out, as the vertex position is in model space, we can just multiply it to grow or shrink it
	vertexPos *= smoothStepInOut( 0.0, 0.1, 0.9, 1.0, ageFrac );
	
	// Rotate the vertex of our mesh (in model space) to face the velocity direction
	mat4 lookAt = makeLookAt( vec3(0,0,0), particleVel, vec3(0,1,0) );
	vertexPos = lookAt * vertexPos;
	
	// We add the rotated model space vertex pos to the particle pos to get the final position in space
	vec3 newVertexPos = particlePos + vertexPos.xyz;
	
	gl_Position = u_modelViewProjectionMatrix * vec4(newVertexPos, 1.0);
	
	// Light stuff
	
	vec3 vertexNormal = gl_Normal;
	
	// Rotate the normal just as we did the vertex, then apply the canera transform
	vertexNormal = (lookAt * vec4(vertexNormal, 0)).xyz;
	v_normal = normalize(gl_NormalMatrix * vertexNormal).xyz;
	
	// We do lighting clculations in view (camera) space
	vec4 viewSpaceVertex = u_modelViewMatrix * vec4(newVertexPos, 1.0);
	v_eyeVec = -viewSpaceVertex.xyz;
	
	for ( int i = 0; i < u_numLights; ++i )
	{
		v_lightDir[i] = vec3(gl_LightSource[i].position.xyz - viewSpaceVertex.xyz);
	}
	
}
