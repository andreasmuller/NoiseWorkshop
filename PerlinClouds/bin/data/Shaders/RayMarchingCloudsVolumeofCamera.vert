
#version 120

uniform float aspect;
uniform float fovYScale;

varying vec3 eyePos;
varying vec3 dir;
varying vec3 cameraForward;

void main(void)
{
	vec4 vertex = gl_Vertex;
	
	eyePos = -(gl_ModelViewMatrix[3].xyz) * mat3(gl_ModelViewMatrix);
	dir = vec3( vertex.x*fovYScale*aspect, vertex.y*fovYScale, -1.0 ) * mat3(gl_ModelViewMatrix);
	cameraForward = vec3(0,0,-1.0)*mat3(gl_ModelViewMatrix);
	
	gl_Position = gl_Vertex;	
}
