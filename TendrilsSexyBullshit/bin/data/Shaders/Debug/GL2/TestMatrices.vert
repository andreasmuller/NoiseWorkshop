#version 120

uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjectionMatrix;

void main()
{
	gl_FrontColor = gl_Color;
	
	//gl_Position = ftransform();
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_Position = u_modelViewProjectionMatrix * gl_Vertex;
	gl_Position = u_projectionMatrix * u_modelViewMatrix * gl_Vertex;
}