

#define MAX_LIGHTS 8

uniform int numLights;

varying vec3 normal, eyeVec;
varying vec3 lightDir[MAX_LIGHTS];

void main()
{
	gl_Position = ftransform();
	
	normal = gl_NormalMatrix * gl_Normal;
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	eyeVec = -vVertex.xyz;
	
	for (int i=0; i<numLights; ++i)
	{
		lightDir[i] = vec3(gl_LightSource[i].position.xyz - vVertex.xyz);
	}
}