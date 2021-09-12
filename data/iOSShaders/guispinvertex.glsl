attribute mediump vec4 position;
attribute lowp vec2 uv1;
uniform highp float Time;
uniform lowp float GuiSpinSpeed;

mat4 orthoProj = mat4(	2, 0, 0, 0,
						0, -2, 0, 0,
						0, 0, -1, 0,
						-1, 1, 0, 1 );

varying lowp vec2 v_uv1;

void main()
{
	gl_Position = orthoProj * position;

	lowp float sinT = sin(Time*GuiSpinSpeed);
	lowp float cosT = cos(Time*GuiSpinSpeed);
	v_uv1.x = (uv1.x - 0.5)*cosT + (uv1.y - 0.5)*sinT + 0.5;
	v_uv1.y = -(uv1.x - 0.5)*sinT + (uv1.y - 0.5)*cosT + 0.5;
}
