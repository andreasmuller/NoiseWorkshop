
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform sampler2D u_opticalFlowMap;

uniform mat4 u_worldToKinect;
uniform float u_fXToZ;
uniform float u_fYToZ;
uniform vec2 u_pixelDepthmap;
uniform float u_maxFlowMag;

// -----------------------------------------------------------
vec2 getFlowVelClamped( vec2 _uv, float _maxMag )
{
	vec2 flow = texture2D( u_opticalFlowMap, _uv).xy;
	float flowLength = length(flow);
	
	if( flowLength > _maxMag )
	{
		flow = (flow/flowLength) * _maxMag;
	}
	
	return flow;
}

// -----------------------------------------------------------
vec2 getFlowVelAverage( vec2 _uv, float _maxMag )
{
	vec2 pixel = u_pixelDepthmap;
	vec2 flow = vec2(0.0,0.0);
	flow += getFlowVelClamped( _uv						 , _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  pixel.x,  0.0 ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2( -pixel.x,  0.0 ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  0.0,  pixel.y ), _maxMag );
	flow += getFlowVelClamped( _uv + vec2(  0.0, -pixel.y ), _maxMag );
	flow /= 5.0;
	
	//flow = texture2D( u_opticalFlowMap, _uv).xy;
	
	return flow;
}

// -----------------------------------------------------------
vec2 getFlowVel( vec3 _worldPos )
{
	vec3 p = (u_worldToKinect * vec4(_worldPos, 1.0)).xyz; // Kinect space pos
	
	float fCoeffX = 1.0 / u_fXToZ;
	float fCoeffY = 1.0 / u_fYToZ;
	
	float nHalfXres = 0.5;
	float nHalfYres = 0.5;
	
	vec2 uvPos;
	uvPos.x = fCoeffX * p.x / p.z + nHalfXres;
	uvPos.y = nHalfYres - fCoeffY * p.y / p.z;
	uvPos.y = 1.0 - uvPos.y;
	
	vec2 flow = getFlowVelAverage( uvPos, u_maxFlowMag );
	flow.y *= -1.0;
	
	return flow;
}


//-------------------------------------------------------------------------------------------------------------------------------------
//
void main()
{
	vec4 color = gl_FrontColorIn[0];
	
	// Let's get the vertices of the line, calculate our stalk up and save how long it was when it came in
	vec4 p0 = gl_PositionIn[0];
	vec4 p1 = gl_PositionIn[1];
	
	//vec2 flowOffset = (p0.xy - p1.xy) * 1.2;
	vec2 flowOffset = getFlowVel( p0.xyz );

	p1 = p0;
	p1.xy += flowOffset;
	p1.xy += vec2(0.05,0.05); // add a litle offset so we draw a dot at least
	
	gl_Position = gl_ModelViewProjectionMatrix * p0;
	gl_FrontColor = color;
	EmitVertex();
	
	gl_Position = gl_ModelViewProjectionMatrix * p1;
	gl_FrontColor = color;
	EmitVertex();

	EndPrimitive();
}
