attribute vec4 position;
attribute vec2 uv1;

mat4 orthoProj = mat4(	2, 0, 0, 0,
						0, -2, 0, 0,
						0, 0, -1, 0,
						-1, 1, 0, 1 );

varying vec2 v_uv1;

void main()
{
    gl_Position = orthoProj * position;
    v_uv1 = uv1;
}
