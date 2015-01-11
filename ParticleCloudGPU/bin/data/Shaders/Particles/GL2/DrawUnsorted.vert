#version 120

#pragma include "Shaders/Common/ShaderHelpers.glslinc"

uniform sampler2D u_particleDataTexture;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjectionMatrix;

uniform float u_particleDiameter;
uniform float u_screenWidth;

uniform float u_particleMaxAge;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

varying vec4 v_particleColor;

// ----------------------------
void main ()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 particleData = texture2D( u_particleDataTexture, texCoord );
	
	vec3 pos = particleData.xyz;
	//pos = vec3( rand( texCoord + pos.xy ), rand( texCoord.xy + pos.yz ), rand( texCoord.yx + pos.yz ));
	
	float frac = particleData.w / u_particleMaxAge;
	
	v_particleColor = mix(u_particleStartColor, u_particleEndColor, frac );
	
	vec3 viewSpacePosition = vec3(u_viewMatrix * vec4(pos, 1.0));
	
	vec4 corner = vec4(u_particleDiameter * 0.5, u_particleDiameter * 0.5, viewSpacePosition.z, 1.0);
	float projectedCornerX = dot(vec4(u_projectionMatrix[0][0], u_projectionMatrix[1][0], u_projectionMatrix[2][0], u_projectionMatrix[3][0]), corner);
	float projectedCornerW = dot(vec4(u_projectionMatrix[0][3], u_projectionMatrix[1][3], u_projectionMatrix[2][3], u_projectionMatrix[3][3]), corner);
	
	gl_PointSize = u_screenWidth * 0.5 * projectedCornerX * 2.0 / projectedCornerW;
	
	gl_Position = u_modelViewProjectionMatrix * vec4(pos, 1.0);
}
