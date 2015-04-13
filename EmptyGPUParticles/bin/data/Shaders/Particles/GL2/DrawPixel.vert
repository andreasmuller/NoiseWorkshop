#version 120

uniform sampler2D u_particleDataTexture;

// ----------------------------
void main ()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 particleData = texture2D( u_particleDataTexture, texCoord );
	vec3 pos = particleData.xyz;
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
	gl_FrontColor = gl_Color;
}
