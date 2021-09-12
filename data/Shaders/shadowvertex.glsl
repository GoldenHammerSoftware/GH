uniform mat4 ModelViewProj;
attribute vec4 position;

varying vec4 v_pos;

void main()
{
    v_pos = ModelViewProj * position;
    gl_Position = v_pos;
}
