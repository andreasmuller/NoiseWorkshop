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

	// Marble
	float angle = modelSpaceVertex.x + 2.0; // shift it a bit so we are dealing only in a positive range 
	angle *= 20.0;
	angle *= fbm( modelSpaceVertex.xyz * map( mouseY, 0, 1, 0, 1.5), 8, 2.0, 0.5 );		
	float tmpMarbleVal = map( sin( angle ), -1, 1,  0, 1 );
	color.xyz = mix( color1.xyz, color2.xyz, tmpMarbleVal );
	// End Marble

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

	// Sun (Not working)
	/*
	vec3 p = modelSpaceVertex.xyz * mouseX * 2.1;

	float noiseVal = abs(snoise(p) 	     - 0.25) +
                     abs(snoise(p * 2.0) - 0.125) +
                     abs(snoise(p * 4.0) - 0.0625) +
                     abs(snoise(p * 18.0) - 0.03125); 

	noiseVal = clamp(noiseVal * 1.0, 0.0, 1.0);
	color.xyz = mix(color1.xyz, color2.xyz, noiseVal);
	*/


	/*
uniform sampler3D Noise;
uniform vec3  Color1;
uniform vec3  Color2;
uniform float NoiseScale;
out vec4 FragColor;
void main() {
// (0.8, 0.7, 0.0)
// (0.6, 0.1, 0.0)
// 1.2
   vec4 noisevec = texture(Noise, MCposition * NoiseScale);
   float intensity = abs(noisevec[0] - 0.25) +
                     abs(noisevec[1] - 0.125) +
                     abs(noisevec[2] - 0.0625) +
                     abs(noisevec[3] - 0.03125);
   intensity = clamp(intensity * 6.0, 0.0, 1.0);
   vec3 color = mix(Color1, Color2, intensity) * LightIntensity;
   FragColor = vec4(color, 1.0);
}

	*/

	// End Sun

	//color.xyz = vec3(mixNoiseVal);
	//color.xyz = vec3(patternNoiseVal);
	//color.xyz = vec3(tmpMarbleVal);	

	//color.xyz = mix( vec3(patternNoiseVal), vec3(0.0), 1.0-mixNoiseVal );
	//color.xyz = mix( vec3(patternNoiseVal), vec3(0.0), 1.0-mixNoiseVal );

	//color = color1;

	//gl_FragColor = vec4( mouseX, mouseY, 1.0, 1.0);
	//gl_FragColor = vec4( normal, 1.0);
	//gl_FragColor = vec4( texCoord.st, 1.0, 1.0);


	gl_FragColor = color;
}