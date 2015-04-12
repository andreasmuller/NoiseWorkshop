precision highp float;

uniform sampler2D u_particleImageTexture;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

varying vec4 v_particleColor;

void main ()
{
	vec4 particleSpriteTexCol = texture2D( u_particleImageTexture, gl_PointCoord.xy );
	
	vec4 color = particleSpriteTexCol * v_particleColor;
	//if (color.a <= 0.0) discard;
	
	gl_FragColor = color;
}