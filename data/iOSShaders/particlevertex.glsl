uniform mediump float MaterialAmbient;
uniform mediump float MaterialEmissive;
uniform mediump mat4 ModelViewProj;
uniform mediump vec4 LightDir0;

attribute mediump vec4 pos;
attribute mediump vec3 normal;
attribute mediump vec2 uv1;
attribute mediump vec4 diffuse;

varying mediump vec3 v_normal;
varying mediump vec2 v_uv;
varying mediump vec4 v_diffuse;

void main()
{
	v_uv = uv1;
	v_normal = normal;
	v_diffuse = diffuse;

    gl_Position = ModelViewProj * pos;
}
