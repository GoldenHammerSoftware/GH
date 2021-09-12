varying vec2 v_uv1;
varying float v_nDotL;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    vec4 color2 = texture2D(Texture1, v_uv1);
    color.xyz = mix(color.xyz, color2.xyz, color2.w);
    color.xyz *= v_nDotL;

    gl_FragColor = color; 
}
