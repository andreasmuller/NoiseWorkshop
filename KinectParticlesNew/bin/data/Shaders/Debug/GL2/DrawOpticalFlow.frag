
uniform sampler2D opticalFlowMap;

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main( void )
{
	//vec2 texCoord = gl_TexCoord[0].st / vec2(320.0,240.0);
	vec2 texCoord = gl_TexCoord[0].st;	
	//vec2 tmpFlow = texture2D(opticalFlowMap, texCoord).xy;
	vec4 tmpFlow = texture2D(opticalFlowMap, texCoord);	

	//gl_FragColor = vec4( tmpFlow.x, tmpFlow.y, 0.0, 1.0);
	gl_FragColor = vec4( tmpFlow.xyz, 0.5);
	//gl_FragColor = vec4( texCoord.x / 320.0, texCoord.y / 240.0, 0.0, 1.0);	
	//gl_FragColor = vec4( texCoord.x, texCoord.y, 0.0, 1.0);		
}
