
#define MAX_LIGHTS 8

uniform int numLights;

varying vec3 normal, eyeVec;
varying vec3 lightDir[MAX_LIGHTS];

void main (void)
{
	vec4 final_color = gl_FrontLightModelProduct.sceneColor;
	vec3 N = normalize(normal);
	
	for ( int i = 0; i < numLights; ++i )
	{
		vec3 L = normalize(lightDir[i]);
		float lambertTerm = dot(N,L);
		
		if (lambertTerm > 0.0)
		{
			final_color += gl_LightSource[i].diffuse * gl_FrontMaterial.diffuse * lambertTerm;
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular = pow(max(dot(R, E), 0.0), gl_FrontMaterial.shininess);
			final_color += gl_LightSource[i].specular * gl_FrontMaterial.specular * specular;
		}
	}
	
	gl_FragColor = final_color;
	
	//gl_FragColor = gl_LightSource[1].diffuse;
	
}

