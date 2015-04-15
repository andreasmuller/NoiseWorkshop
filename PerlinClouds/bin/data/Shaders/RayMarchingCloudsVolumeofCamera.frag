#version 120

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// Butchered by Andreas Müller 2015


uniform vec2 resolution;
uniform float time;		// time in seconds

uniform sampler2D noiseTexture;

uniform float frequency;

uniform vec4 cloudBaseColor;

varying vec3 eyePos;
varying vec3 dir;
varying vec3 cameraForward;


// Never leave home without these
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
float map( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }
float mapClamped( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin),    outputMin, outputMax ); }
float stepInOut( float _edge1, float _edge2, float _val ) { return step(_edge1, _val) - step(_edge2,_val); }
float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return smoothstep( _low0, _high0, _t ) * (1.0 - smoothstep( _high1, _low1, _t )); }
vec3  mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2  mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3  permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }


// Uncomment the line below to compute noise without a texture lookup, it's slower now but maybe not on some strange device you are porting it to
//#define FULL_PROCEDURAL

#ifdef FULL_PROCEDURAL

	// ---------------------------------------------
	// hash based 3d value noise
	float hash( float n )
	{
		return fract(sin(n)*43758.5453);
	}

	// ---------------------------------------------	
	float noise( in vec3 x )
	{
		vec3 p = floor(x);
		vec3 f = fract(x);

		f = f*f*(3.0-2.0*f);
		float n = p.x + p.y*57.0 + 113.0*p.z;
		return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
					   mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
				   mix(mix( hash(n+113.0), hash(n+114.0),f.x),
					   mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
	}
#else

	// ---------------------------------------------
	// LUT based 3d value noise
	float noise( in vec3 x )
	{
		vec3 p = floor(x);
		vec3 f = fract(x);
		f = f*f*(3.0-2.0*f);
		
		vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
		vec2 rg = texture2D( noiseTexture, (uv+ 0.5)/256.0, -100.0 ).yx;
		return mix( rg.x, rg.y, f.z );
	}
#endif

// ---------------------------------------------
struct Ray
{
    vec3 Origin;
    vec3 Dir;
};

// ---------------------------------------------
struct AABB
{
    vec3 Min;
    vec3 Max;
};

// ---------------------------------------------
bool IntersectBox(Ray r, AABB aabb, out float t0, out float t1)
{
    vec3 invR = 1.0 / r.Dir;
    vec3 tbot = invR * (aabb.Min-r.Origin);
    vec3 ttop = invR * (aabb.Max-r.Origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}

// ---------------------------------------------
float sdSphere( vec3 p, float s )
{
	return length(p)-s;
}

// ---------------------------------------------
float udBox( vec3 p, vec3 b )
{
	return length(max(abs(p)-b,0.0));
}

// ---------------------------------------------
vec4 map( in vec3 p )
{
	
	float d = -abs(0.9 - p.y);	// this is some sort of starting point we later sum up the noise to, so we come up with a function that begins low enough towards the edges of the volume

	vec3 q = p + vec3(1.0,0.0,0.0)*time;
	q *= frequency;
	
	float f;
    f  = 0.5000*noise( q ); q = q*2.02;
    f += 0.2500*noise( q ); q = q*2.03;
    f += 0.1250*noise( q ); q = q*2.01;
    f += 0.0625*noise( q );

	//d += 3.0 * f;
	d += 1.1 * f; // Boost the values a tad
	//d += f;
	//d = f;

	
	//d = step( sdSphere( p, 1.0 ), 1.0);
	//d = step( udBox( p, vec3( 1.0 ) ), 1.0 );
	//d = step( length(p), 1.0 );
	//d = step( noise( q ), 0.1 );
	
	d = clamp( d, 0.0, 1.0 );
	d *= noise( q * 0.10 ); // multiply by a lower frequency noise to make clouds more sparse
	vec4 res = vec4( d );
	
	return res;
}


vec3 sundir = vec3(-1.0,0.0,0.0);

// ---------------------------------------------
vec4 raymarch( in vec3 ro, in vec3 rd, in float startDist, in float endDist )
{
	vec4 sum = vec4(0, 0, 0, 0);

	float t = startDist;
	for( int i = 0; i < 128; i++ ) // For a smoother raymarch, up the number of steps
	{
		if( t > endDist )
		{
			continue;
		}

		if( sum.a > 0.99 ) continue;

		vec3 pos = ro + t*rd;
		vec4 col = map( pos );
		
		#if 0
			float dif =  clamp((col.w - map(pos+0.3*sundir).w)/0.6, 0.0, 1.0 );
	        vec3 lin = vec3(0.65,0.68,0.7)*1.35 + 0.45*vec3(0.7, 0.5, 0.3)*dif;
			col.xyz *= lin;
		#endif
		
		col.a *= 0.35;
		col.rgb *= col.a;

		sum = sum + col*(1.0 - sum.a);	

        #if 0
			t += 0.05;
		#else
			//t += max(0.1,0.025*t);
			t += max(0.05,0.012*t);			
		#endif
	}

	sum.xyz /= (0.001+sum.w);

	return clamp( sum, 0.0, 1.0 );
}

// ---------------------------------------------
void main(void)
{
	vec2 q = gl_FragCoord.xy / resolution.xy;
    vec2 p = -1.0 + 2.0*q;
    p.x *= resolution.x / resolution.y;
	
	vec3 rayDirection = normalize(dir);
	Ray eyeRay = Ray( eyePos, rayDirection );
	
    //AABB aabb = AABB(vec3(-1.0), vec3(+1.0));
	AABB aabb = AABB(vec3(-20,-2,-20), vec3(20,2,20)); // we optimize by only raymarching within this bounding box

	float tnear, tfar;
    bool doesIntersect = IntersectBox(eyeRay, aabb, tnear, tfar);
    if (tnear < 0.0) tnear = 0.0;
	
	if( doesIntersect )
	{
		vec4 res = raymarch( eyePos, rayDirection, tnear, tfar );
		float sun = clamp( dot(sundir,rayDirection), 0.0, 1.0 );

		/*
		vec3 col = vec3(0.6,0.71,0.75) - rayDirection.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
		col += 0.2*vec3(1.0,.6,0.1)*pow( sun, 8.0 );
		col *= 0.95;
		col = mix( col, res.xyz, res.w );
		col += 0.1*vec3(1.0,0.4,0.2)*pow( sun, 3.0 );
		gl_FragColor = vec4( col, 1.0 ); 
		*/

//		gl_FragColor = vec4( cloudBaseColor.rgb*pow( sun, 8.0 ), res.x  );	
		gl_FragColor = vec4( cloudBaseColor.rgb, res.x * cloudBaseColor.a );
	}
	else
	{
		//gl_FragColor = vec4( 0,0,0,0 );
		discard;
	}
	
}
