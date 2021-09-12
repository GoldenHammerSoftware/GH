uniform mat4 ModelViewProj;

attribute vec4 position;
attribute vec2 uv1;
attribute vec3 normal;
attribute vec4 diffuse;

varying vec2 v_uv1;
varying vec4 v_color;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
    v_color = diffuse/255.0;
}
