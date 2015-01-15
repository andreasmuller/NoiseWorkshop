
#version 120

uniform float aspect;
uniform float fov_y_scale;

varying vec3 eyePos;
varying vec3 dir;
varying vec3 cameraForward;

void main(void)
{
	//gl_Position = ftransform();
	gl_Position = gl_Vertex;
	
	vec4 vertex = gl_Vertex;
	
	eyePos = -(gl_ModelViewMatrix[3].xyz) * mat3(gl_ModelViewMatrix);
	dir = vec3( vertex.x*fov_y_scale*aspect, vertex.y*fov_y_scale, -1.0 ) * mat3(gl_ModelViewMatrix);
	cameraForward = vec3(0,0,-1.0)*mat3(gl_ModelViewMatrix);
}
