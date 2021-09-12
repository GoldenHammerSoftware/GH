uniform mat4 ModelViewProj;
uniform mat4 ModelToWorld;
uniform vec3 movecircle;
uniform float TextureWorldSize;

attribute vec4 position;

varying vec2 v_uv1;
varying vec2 v_uv2;

varying vec4 v_position;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1.x = position.x / TextureWorldSize;
    v_uv1.y = position.z / TextureWorldSize;

    v_position = ModelToWorld * position;
    v_uv2.x = ((movecircle.x-v_position.x)/movecircle.z)*0.5+0.5;
    v_uv2.y = ((movecircle.y+v_position.z)/movecircle.z)*0.5+0.5;
}

