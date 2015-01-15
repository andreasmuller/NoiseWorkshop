#version 120
#extension GL_EXT_gpu_shader4 : require

//uniform sampler2D u_particleImageTexture;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

varying vec4 v_particleColor;

void main ()
{
	vec4 color = v_particleColor;
	gl_FragColor = color;
}