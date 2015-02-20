varying vec4 v_Position;

void main( void )
{
    v_Position = gl_ModelViewMatrix * gl_Vertex;
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;	
}