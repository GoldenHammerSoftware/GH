uniform highp mat4 ModelViewProj;
attribute highp vec4 position;

varying highp vec4 v_pos;

void main()
{
    v_pos = ModelViewProj * position;
    gl_Position = v_pos;
}
