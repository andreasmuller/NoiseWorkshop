#version 120

#pragma include "Shaders/Common/ShaderHelpers.glslinc"
#pragma include "Shaders/Common/Noise2D.glslinc"
#pragma include "Shaders/Common/Noise3D.glslinc"
#pragma include "Shaders/Common/Noise4D.glslinc"

uniform float time;

uniform float mouseX;	// normalized mouse coordinates
uniform float mouseY;

uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;

uniform float shininess; // let the material handle this? I guess once we have lighting going 

varying vec3 normal;
varying vec3 eyeDir;
varying vec3 lightDir;
varying vec3 modelSpaceVertex;

float pattern(in vec2 p) {
    float l = 2.5;
    float g = 0.4;
    int oc = 10;
    
    vec2 q = vec2( fbm( p + vec2(0.0,0.0),oc,l,g),fbm( p + vec2(5.2,1.3),oc,l,g));
    vec2 r = vec2( fbm( p + 4.0*q + vec2(1.7,9.2),oc,l,g ), fbm( p + 4.0*q + vec2(8.3,2.8) ,oc,l,g));
    return fbm( p + 4.0*r ,oc,l,g);    
}

void main()
{
	vec2 texCoord = gl_TexCoord[0].st;
	
	vec4 color = vec4( 1.0, 1.0, 1.0, 1.0 );

	// Do one that just thresholds a bit

	/*
	// Clouds
	vec3 p = modelSpaceVertex.xyz;
	p += vec3(time * 0.14, 0, time * 0.08);// make it move, we can also look up a 4D noise with vec4(x,y,z,time)
	p *= 1.4f; //mouseX * 3.0; // change frequency
	float noiseVal = map( fbm( p, 12, 2.0, 0.5 ), -1, 1, -0.4, 1.0); // note we map to -0.3 to 1.0 to boost the higher values a bit
	color.xyz = mix( color1.xyz, color2.xyz, noiseVal );	
	// End Clouds
	*/

	/*
	// Marble
	float angle = modelSpaceVertex.x + 2.0; // shift it a bit so we are dealing only in a positive range 
	angle *= 20.0;
	angle *= fbm( modelSpaceVertex.xyz * map( mouseY, 0, 1, 0, 1.5), 8, 2.0, 0.5 );		
	float tmpMarbleVal = map( sin( angle ), -1, 1,  0, 1 );
	color.xyz = mix( color1.xyz, color2.xyz, tmpMarbleVal );
	// End Marble
	*/


	/*
	// Lattice
	vec2 scale = vec2( mouseX * 10);
	vec2 threshold = vec2( 0.1, 0.1);
 	float ss = fract(texCoord.s * scale.s);
    float tt = fract(texCoord.t * scale.t);
    if ((ss > threshold.s) && (tt > threshold.t)) discard;
    color.xyz = vec3(1.0);
    // End lattice
	*/

	// Burn away
	vec3 p = modelSpaceVertex.xyz;
	p += vec3(time * 0.14, 0, time * 0.08);// make it move, we can also look up a 4D noise with vec4(x,y,z,time)
	p *= 1.4f; //mouseX * 3.0; // change frequency
	float noiseVal = map( fbm( p, 12, 2.0, 0.5 ), -1, 1,  0, 1.0); // note we map to -0.3 to 1.0 to boost the higher values a bit
	vec3 surfaceColor = mix( color1.xyz, color2.xyz, noiseVal ); // surface color could be anything
	noiseVal -= mod(time, 10.0) / 10.0; // substract an increasing number from the noiseVal
	noiseVal = smoothstep( 0.05, 0.95, noiseVal ); // let's keep it at 0 and 1 a little bit longer, also gives it a bit of easing
	if (noiseVal <= 0) discard;
	float burnAmount = linearStep( 0.0, 0.1, noiseVal ); // we have a value from 0..1, let's map it so the first 10% ramps up to 1 and holds there   
	//float burnAmount = step( 0.05, noiseVal );	
	vec3 burnColor = mix( vec3(0), color3.xyz, burnAmount );
	color.xyz = mix( surfaceColor, burnColor, 1.0-step( 0.1, noiseVal )); 	
	//color.xyz = surfaceColor;	
	//color.xyz = burnColor;		
	// End Burn away
	


	/*
	// Granite
 	vec3 p = modelSpaceVertex.xyz;
 	p += vec3(2,0,0);
 	float freq = mouseX * 6.0;
 	p *= vec3( freq);
 	float noiseVal  = fbm( p, 7, 2.0, 0.5 ); 
 	noiseVal = mapClamped( noiseVal * 1.6, -1.0, 1.0, -0.8, 1.7 );	
   	color.xyz = mix( color1.xyz, color2.xyz, noiseVal );
	// End Granite
	*/

	/*
	// Dots
	vec3 spacing = vec3(0.2, 0.2, 0.18 );
	float dotSize = 0.09;
	vec3 p = mod( modelSpaceVertex.xyz, spacing );
	p = p - (spacing * 0.5); // center
	float scaledPointLength = length(p);
	float insideDot = step( scaledPointLength, dotSize ); 
	color.xyz = mix(color1.xyz, color2.xyz, insideDot);
	*/

	/*
	// Sun (Not working)
	vec3 p = modelSpaceVertex.xyz * mouseX * 2.1;

	float noiseVal = abs(snoise(p) 	     - 0.25) +
                     abs(snoise(p * 2.0) - 0.125) +
                     abs(snoise(p * 4.0) - 0.0625) +
                     abs(snoise(p * 18.0) - 0.03125); 

	noiseVal = clamp(noiseVal * 1.0, 0.0, 1.0);
	color.xyz = mix(color1.xyz, color2.xyz, noiseVal);
	// End Sun
	*/

	gl_FragColor = color;
}