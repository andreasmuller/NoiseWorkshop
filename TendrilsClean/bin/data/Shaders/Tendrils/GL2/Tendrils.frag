#version 120

varying vec4 out_normal;

void main (void)
{
	gl_FragColor = gl_Color;
	//gl_FragColor = vec4((out_normal.xyz + vec3(1.0)) * 0.5, 1.0);
	//gl_FragColor = vec4(0.0,1.0,0.0,1.0);
}