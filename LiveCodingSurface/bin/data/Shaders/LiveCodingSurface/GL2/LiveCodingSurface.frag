#version 120

#pragma include "Shaders/Common/ShaderHelpers.glslinc"
#pragma include "Shaders/Common/Noise3D.glslinc"
#pragma include "Shaders/Common/Noise4D.glslinc"

uniform float time;

uniform float mouseX;	// normalized mouse coordinates
uniform float mouseY;

varying vec3 normal;
varying vec3 eyeDir;
varying vec3 lightDir;
varying vec3 modelSpaceVertex;

void main()
{
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 color = vec4( 1.0, 1.0, 1.0, 1.0 );
	//color.x = map( sin(texCoord.x * 0.01), -1, 1,  0, 1 );
	//color.y = map( cos(texCoord.y * 0.01), -1, 1,  0, 1 );

	//color.x = map( sin( time + texCoord.t * (240.0 * mouseX) ), -1, 1,  1, 0 );
	//color.y = map( sin( time + texCoord.s * (200.0 * mouseY) ), -1, 1,  1, 0 );

	//color.x = snoise( modelSpaceVertex.xyz * mouseX );
	//color.y = snoise( modelSpaceVertex.yxz * mouseY );

	float noiseVal = map( snoise( modelSpaceVertex.xyz * mouseX * 10.0 ), -1, 1, 0, 1);
	//noiseVal = stepInOut( 0.4, 0.6, noiseVal );
	noiseVal = linearStepInOut( 0.4, 0.6, 0.7, 0.8, noiseVal );	
//	
	color.xyz = vec3(noiseVal);

	//gl_FragColor = vec4( mouseX, mouseY, 1.0, 1.0);
	//gl_FragColor = vec4( normal, 1.0);
	//gl_FragColor = vec4( texCoord.st, 1.0, 1.0);



	gl_FragColor = color;
}