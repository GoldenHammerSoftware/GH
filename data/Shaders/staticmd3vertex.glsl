uniform mat4 ModelViewProj;
uniform vec3 LightDir0;
uniform float MaterialEmissive;
uniform float MaterialAmbient;

attribute vec4 position;
attribute vec2 uv1;
attribute vec3 normal;

varying vec2 v_uv1;
varying float v_nDotL;

void main()
{
    gl_Position = ModelViewProj * position;
    v_uv1 = uv1;
       
    float LightAmbientAmount = 0.25 * (1.0-MaterialAmbient) + MaterialEmissive;
    v_nDotL = max(dot(LightDir0, normal), 0.0);
	v_nDotL += LightAmbientAmount;
	v_nDotL = min(v_nDotL, 1.0);
}
