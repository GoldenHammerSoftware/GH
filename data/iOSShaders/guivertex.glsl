#include "guivertexinclude.glsl"

attribute mediump vec4 position;
attribute lowp vec2 uv1;

varying lowp vec2 v_uv1;

void main()
{
	gl_Position = positionGUIVertex(position.xy);
    v_uv1 = uv1;
}
