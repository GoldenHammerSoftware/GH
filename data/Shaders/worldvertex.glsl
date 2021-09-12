uniform mat4 ModelViewProj;

attribute vec4 position;
attribute vec2 uv1;

varying vec2 v_uv1;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
}
