uniform mat4 ModelViewProj;
uniform mat4 ModelToWorld;
uniform vec3 movecircle;

attribute vec4 position;
attribute vec2 uv1;

varying vec2 v_uv1;
varying vec2 v_uv2;

varying vec4 v_position;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;

    v_uv2 = ((movecircle.xy-position.xy)/movecircle.z)*0.5+0.5;
    
    v_position = ModelToWorld * position;
}
