uniform mat4 ModelViewProj;
uniform vec2 DiffuseUVScale;

attribute mediump vec4 position;
attribute mediump vec3 normal;
attribute mediump vec2 uv1;
attribute mediump vec2 uv2;

varying mediump vec2 v_uv;

void main()
{
	gl_Position = ModelViewProj * position;

	v_uv = uv1 * DiffuseUVScale;
}
