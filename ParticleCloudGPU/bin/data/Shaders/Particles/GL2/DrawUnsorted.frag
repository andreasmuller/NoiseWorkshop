#version 120

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

varying vec4 v_particleColor;

void main ()
{
	float distanceFromCenter = distance(gl_PointCoord.xy, vec2(0.5, 0.5));
	if (distanceFromCenter > 0.5) discard;
	
	vec4 particleColor = v_particleColor;
	
	float distanceFromCenterAlpha = clamp(1.0 - distanceFromCenter * 2.0, 0.0, 1.0);
	
	vec4 color;
	color.rgb = particleColor.rgb; // So this is darkening from alpha of the opacity map. Todo: look into changing this
	//color.rgb *= distanceFromCenterAlpha;
	color.a = particleColor.a * distanceFromCenterAlpha;
	//color.a = 1.0;
	
	gl_FragColor = color;
	//gl_FragColor = vec4(1.0,1.0,1.0,1.0);

}