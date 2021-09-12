uniform mat4 ModelViewProj;

attribute mediump vec4 position;
attribute mediump vec2 uv1;

varying mediump vec2 v_uv1;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
}
