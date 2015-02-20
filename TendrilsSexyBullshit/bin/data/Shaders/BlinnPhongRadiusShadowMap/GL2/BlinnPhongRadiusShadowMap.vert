#version 120

// This could be anything, no need to stick to max 8
#define MAX_LIGHTS 4

uniform int	numActiveLights;

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

uniform mat4  toShadowSpaceMatrix;

varying vec4 vertShadowTexSpace;
varying vec4 vertEyeSpace;
varying vec3 normal;
varying vec3 viewDir;

varying vec3 lightDir[MAX_LIGHTS];


void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	vertEyeSpace = gl_ModelViewMatrix * gl_Vertex;
	viewDir = -vertEyeSpace.xyz;
	
	for ( int i = 0; i < numActiveLights; i++ )
	{
		lightDir[i] = vec3(lightPositionCamera[i] - vertEyeSpace.xyz) / lightRadius[i];
	}
	
	// We should sure that the vertex is in world space first?
	// We can do this with the modelView matrix in the GL3 path, this will be set by OF for each 'object' we draw
	
	vertShadowTexSpace = toShadowSpaceMatrix * vertEyeSpace;
	//vertShadowTexSpace = gl_Vertex * toShadowSpaceMatrix;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}


