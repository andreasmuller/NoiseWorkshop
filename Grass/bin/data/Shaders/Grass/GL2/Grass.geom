
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float timeSecs;

uniform vec3 cameraWorldPos;

uniform float stalkHalfWidth;
uniform float stalkHeight;
uniform float stalkSwayingMaxAngle;

uniform float animationTimeMaxDifference;

uniform float grassSwayingNoiseFrequency;
uniform float grassSwayingTimeScale;

//-------------------------------------------------------------------------------------------------------------------------------------
//
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
float map( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }
float mapClamped( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin),    outputMin, outputMax ); }
float stepInOut( float _edge1, float _edge2, float _val ) { return step(_edge1, _val) - step(_edge2,_val); }
float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return smoothstep( _low0, _high0, _t ) * (1.0 - smoothstep( _high1, _low1, _t )); }
vec3  mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2  mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3  permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

//-------------------------------------------------------------------------------------------------------------------------------------
//
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
float snoise(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	
	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	
	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
					 + i.x + vec3(0.0, i1.x, 1.0 ));
	
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	
	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}



//-------------------------------------------------------------------------------------------------------------------------------------
//
void main()
{
	vec4 color = gl_FrontColorIn[0];
	
	// Let's get the vertices of the line, calculate our stalk up and save how long it was when it came in
	vec4 p0 = gl_PositionIn[0];
	vec4 p1 = gl_PositionIn[1];
	vec4 stalkUp = p1 - p0;
	float stalkOrigLength = length(stalkUp);
	stalkUp = stalkUp / stalkOrigLength;
	
	// Screen-aligned axes, could come in handy
	vec3 right		= normalize(vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]));
	vec3 up			= normalize(vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]));
	vec3 forward    = normalize(vec3(gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2]));
	
	float stalkLength = stalkHeight * stalkOrigLength;
	
	// Lets calculate two angles and create a swaying rotation matrix for each stalk
	float swayTime = timeSecs * grassSwayingTimeScale;
	
	// The noise is in the range -1..1
	float swayNoise1 = snoise(  p0.xz                        * grassSwayingNoiseFrequency);
	float swayNoise2 = snoise( (p0.xz + vec2(123.4f,567.8f)) * grassSwayingNoiseFrequency);
	
	float timeOffset1 = swayNoise1 * animationTimeMaxDifference;
	float timeOffset2 = swayNoise2 * animationTimeMaxDifference;

	float swayingAngleRad1 = sin(swayTime + timeOffset1) * stalkSwayingMaxAngle;
	float swayingAngleRad2 = sin(swayTime + timeOffset2) * stalkSwayingMaxAngle;

	mat4 swayingMat = rotationMatrix( vec3(1,0,0), swayingAngleRad1 ) * rotationMatrix( vec3(0,0,1), swayingAngleRad2 );
	
	color.r = map( sin(swayTime + timeOffset1), -1.0, 1.0, 0.0, 1.0 );
	color.g = map( cos(swayTime + timeOffset2), -1.0, 1.0, 0.0, 1.0 );
	color.b = 1.0 - color.g; // this could look incredible, try replacing it with a palette texture.
	
	vec3 stalkSide = right * stalkHalfWidth;
	vec4 stalkVertexPos = p0; // + vec4( (right*tmpRand*randomMovementMagnitude) + (up*tmpRand*randomMovementMagnitude), 1.0);
	
	const int vertexOutAmount = 7; // not making this a uniform for now, could be a bit less compatible, non static loop lengths
	float stepMagnitude = stalkLength / float(vertexOutAmount - 1);
	
	for( int i = 0; i < vertexOutAmount; i++ )
	{
		float tmpFrac = float(i) / float(vertexOutAmount-1);
		
		// Let's add a bit of fake light by darkening the stalk towards the bottom
		float darkenAmount = map( smoothstep( 0.4, 1.0, tmpFrac ), 0.0, 1.0,		0.3, 1.0 );
		vec4 darkenColor = vec4( darkenAmount, darkenAmount, darkenAmount, 1.0 );
		
		vec3 tmpStalkHalfSide = stalkSide * (1.0-tmpFrac);
		vec3 tmpStalkNormal = cross( normalize(tmpStalkHalfSide), normalize(stalkUp.xyz) );
		
		gl_Position = gl_ModelViewProjectionMatrix * vec4((stalkVertexPos.xyz - tmpStalkHalfSide), 1.0);
//		gl_Normal = tmpStalkNormal;
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * vec4((stalkVertexPos.xyz + tmpStalkHalfSide), 1.0);
//		gl_Normal = tmpStalkNormal;
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		stalkUp = stalkUp * swayingMat;
		stalkVertexPos += stalkUp * stepMagnitude;
	}
}
