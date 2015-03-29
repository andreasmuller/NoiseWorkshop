
uniform sampler2D opticalFlowMap;

//-------------------------------------------------------------------------------------------------------------------------------------
// Thanks hughsk! https://github.com/hughsk/glsl-hsv2rgb
// All of the values should range between 0 and 1. Returns the calculated RGB value as a vec3.
//
vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main( void )
{
	//vec2 texCoord = gl_TexCoord[0].st / vec2(320.0,240.0);
	vec2 texCoord = gl_TexCoord[0].st;	
	//vec2 tmpFlow = texture2D(opticalFlowMap, texCoord).xy;
	vec4 tmpFlow = texture2D(opticalFlowMap, texCoord);
	
	float flowMagnitude = length(tmpFlow.xy);	
	vec2 flowNorm = tmpFlow.xy / flowMagnitude;

	//vec2 tmpNormCentered = (tmpFlow.xy + vec2(1.0,1.0)) * 0.5;
	vec2 tmpNormCentered = (flowNorm.xy + vec2(1.0,1.0)) * 0.5;
	//vec2 tmpNormCentered = vec2(0.0, 0.0);
	//tmpNormCentered.x = abs(tmpFlow.x);// + 1.0;// * 0.5;
	//tmpNormCentered.x = abs((texCoord.x - 0.5)*2.0);// + 1.0;// * 0.5;
	
	//vec3 floColHSV = vec3( atan(flowNorm.y, flowNorm.x), 1.0, 1.0 );
	//vec3 floColHSV = vec3( atan(tmpNormCentered.y, tmpNormCentered.x), 1.0, 1.0 );
	//vec4 flowCol = vec4( hsv2rgb(floColHSV), 1.0 );

	//gl_FragColor = vec4( tmpFlow.x, tmpFlow.y, 0.0, 1.0);
	gl_FragColor = vec4( tmpNormCentered.x, tmpNormCentered.y, 0.0 ,1.0);
	//gl_FragColor = flowCol;
	//gl_FragColor = vec4( texCoord.x / 320.0, texCoord.y / 240.0, 0.0, 1.0);	
	//gl_FragColor = vec4( texCoord.x, texCoord.y, 0.0, 1.0);		
}
