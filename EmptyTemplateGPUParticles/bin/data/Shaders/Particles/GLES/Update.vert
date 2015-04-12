precision highp float;

attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
attribute vec2 texcoord;

uniform mat4 modelViewProjectionMatrix;

varying vec4 colorVarying;
varying vec2 texCoordVarying;

// -----------------------------------------------------------
void main()
{

	colorVarying = color;
	texCoordVarying = texcoord;
	
	gl_Position = modelViewProjectionMatrix * position;
}

