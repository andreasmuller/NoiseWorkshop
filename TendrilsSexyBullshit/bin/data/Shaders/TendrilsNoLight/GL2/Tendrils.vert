#version 120

//varying vec3 normal;

void main()
{
	gl_FrontColor =  gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_Vertex;
}
