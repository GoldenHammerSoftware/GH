uniform mat4 ModelViewProj;

attribute lowp vec4 position;
attribute lowp vec4 diffuse;

varying vec4 v_diffuse;

void main()
{
    gl_Position = ModelViewProj * position;
    v_diffuse = diffuse;
}
