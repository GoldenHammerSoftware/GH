uniform mat4 ModelViewProj;
uniform float TextureWorldSize;

attribute vec4 position;

varying vec2 v_uv1;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1.x = position.x / TextureWorldSize;
    v_uv1.y = position.z / TextureWorldSize;
}
