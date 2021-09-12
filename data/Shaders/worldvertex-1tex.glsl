uniform mat4 ModelViewProj;

attribute vec4 position;
attribute vec2 uv1;
attribute vec3 normal;

varying vec2 v_uv1;
varying vec3 v_normal;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
    v_normal = normal;
}
