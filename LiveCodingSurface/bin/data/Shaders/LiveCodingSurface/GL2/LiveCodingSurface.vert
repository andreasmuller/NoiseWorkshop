#version 120

uniform float mouseX;	// normalized mouse coordinates
uniform float mouseY;


varying vec3 normal;
varying vec3 eyeDir;
varying vec3 lightDir;
varying vec3 modelSpaceVertex;

void main()
{
	vec4 vertexEyeSpace = gl_ModelViewMatrix * gl_Vertex;
	eyeDir = -vertexEyeSpace.xyz;
	
	lightDir = vec3(gl_LightSource[0].position.xyz - vertexEyeSpace.xyz);

	modelSpaceVertex = gl_Vertex.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	normal = gl_NormalMatrix * gl_Normal;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}