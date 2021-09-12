uniform mat4 ModelViewProj;

attribute mediump vec4 position;
attribute mediump vec2 uv1;
attribute mediump vec3 normal;

varying mediump vec2 v_uv1;
varying mediump vec3 v_normal;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
    v_normal = normal;
}
