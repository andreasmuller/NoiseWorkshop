#version 120
#extension GL_EXT_gpu_shader4 : require

//uniform sampler2D u_particleImageTexture;

uniform vec4 u_particleStartColor;
uniform vec4 u_particleEndColor;

uniform int u_numLights = 0;

varying vec3 v_normal;
varying vec3 v_eyeVec;

#define MAX_LIGHTS 8
varying vec3 v_lightDir[MAX_LIGHTS];

varying vec4 v_particleColor;

// --------------------------------------------
vec4 computeLight()
{
	vec4 finalColor = gl_FrontLightModelProduct.sceneColor;
	vec3 N = normalize(v_normal); // The normal will be interpolated between vertices, so we need to normalize again just in case.
	vec3 E = normalize(v_eyeVec);
	
	float tmpSaved = 0.0;
	
	for ( int i = 0; i < u_numLights; ++i )
	{
		vec3 L = normalize( v_lightDir[i] );
		float lambertTerm = dot(N,L);
		
		//tmpSaved = lambertTerm; //clamp(lambertTerm, 0.0, 1.0);
		
		if (lambertTerm > 0.0)
		{
			//finalColor += gl_LightSource[i].diffuse * gl_FrontMaterial.diffuse * lambertTerm;
			finalColor += gl_LightSource[i].diffuse * v_particleColor * lambertTerm; // we would normally use gl_FrontMaterial.diffuse instead of v_particleColor if we were just using a material
			
			vec3 R = reflect(-L, N);
			float specular = pow(max(dot(R, E), 0.0), gl_FrontMaterial.shininess);
			
			tmpSaved = specular; //clamp(specular, 0.0, 1.0);
			
			finalColor += gl_LightSource[i].specular * gl_FrontMaterial.specular * specular;
		}
	}

	//return vec4(tmpSaved,tmpSaved,tmpSaved,1.0);
	return finalColor;
}

// --------------------------------------------
void main ()
{
	vec4 color = computeLight();
	gl_FragColor = color;
	//gl_FragColor = vec4( v_normal.xyz, 1.0 );
	
}