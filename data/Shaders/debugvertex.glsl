uniform mat4 ModelViewProj;

attribute vec4 position;
attribute vec4 diffuse;

varying vec4 v_diffuse;

void main()
{
    gl_Position = ModelViewProj * position;
    v_diffuse = diffuse;
}
