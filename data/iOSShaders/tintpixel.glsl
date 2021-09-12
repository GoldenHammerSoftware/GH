uniform sampler2D Texture0;
uniform mediump vec3 Tint;

varying lowp vec2 v_uv1;
varying lowp float v_nDotL;

void main()
{
    mediump vec4 texelColor = texture2D(Texture0, v_uv1);
    texelColor.xyz *= Tint;
    texelColor.xyz *= v_nDotL;
    gl_FragColor = texelColor;
}
