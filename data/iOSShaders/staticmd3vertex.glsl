uniform mat4 ModelViewProj;
uniform mediump vec3 LightDir0;
uniform lowp float MaterialAmbient;
uniform lowp float MaterialEmissive;

attribute mediump vec4 position;
attribute mediump vec2 uv1;
attribute mediump vec3 normal;

varying lowp vec2 v_uv1;
varying lowp float v_nDotL;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;

    float LightAmbientAmount = 0.25 * (1.0-MaterialAmbient) + MaterialEmissive;
    v_nDotL = max(dot(LightDir0, normal), 0.0);
	v_nDotL += LightAmbientAmount;
	v_nDotL = min(v_nDotL, 1.0);
}
