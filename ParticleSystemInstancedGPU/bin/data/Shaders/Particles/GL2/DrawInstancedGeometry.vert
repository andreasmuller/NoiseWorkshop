#version 120
#extension GL_EXT_gpu_shader4 : require

#pragma include "Shaders/Common/ShaderHelpers.glslinc"

uniform vec2 u_resolution;

uniform float u_time;

uniform sampler2D u_particlePosAndAgeTexture;
uniform sampler2D u_particleVelTexture;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjectionMatrix;

uniform float u_particleDiameter;
uniform float u_screenWidth;

uniform float u_particleMaxAge;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

varying vec4 v_particleColor;

/*
// ----------------------------
mat4 makeLookAt(vec3 pos, vec3 at, vec3 up)
{
	mat4 M;
	
	vec3 ZAxis = at - pos;
	normalize(ZAxis);
	
	vec3 XAxis = cross(up,ZAxis);
	normalize(XAxis);
	
	vec3 YAxis = cross(ZAxis,XAxis);
	
	M[0] = vec4(XAxis.x,YAxis.x,ZAxis.x,0);
	M[1] = vec4(XAxis.y,YAxis.y,ZAxis.y,0);
	M[2] = vec4(XAxis.z,YAxis.z,ZAxis.z,0);
	M[3] = vec4(dot(-XAxis,pos),dot(-YAxis,pos),dot(-ZAxis,pos),1);
	
	return M;
}
*/

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

/*
 
void ofMatrix4x4::makeLookAtMatrix(const ofVec3f& eye,const ofVec3f& center,const ofVec3f& up)
{
	ofVec3f zaxis = (eye - center).normalized();
	ofVec3f xaxis = up.getCrossed(zaxis).normalized();
	ofVec3f yaxis = zaxis.getCrossed(xaxis);

	_mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
	_mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
	_mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);
	_mat[3] = eye;
}

 
 */

// ----------------------------
void main ()
{
	vec2 texCoord;
	texCoord.x = mod(gl_InstanceID, u_resolution.x) / u_resolution.x;
	texCoord.y = floor(gl_InstanceID / u_resolution.x) / u_resolution.y;
	
	vec4 particleData = texture2D( u_particlePosAndAgeTexture, texCoord );
	vec3 particleVel = texture2D( u_particleVelTexture, texCoord ).xyz;
	
	vec3 pos = particleData.xyz;
	
	vec4 vertexPos = gl_Vertex;
	//mat4 lookAt = makeLookAt( vec3(0,0,0), vec3( cos(u_time)*2.0, cos(u_time)*2.0, sin(u_time)*2.0), vec3(0,1,0) );
	mat4 lookAt = makeLookAt( vec3(0,0,0), particleVel, vec3(0,1,0) );
	//mat4 lookAt = makeLookAt( pos, pos + particleVel, vec3(0,1,0) );
	
	vertexPos = lookAt * vertexPos;
	
	pos = pos + vertexPos.xyz;
	
	float frac = particleData.w / u_particleMaxAge;
	
	v_particleColor = mix(u_particleStartColor, u_particleEndColor, frac );
	
	gl_Position = u_modelViewProjectionMatrix * vec4(pos, 1.0);
}
