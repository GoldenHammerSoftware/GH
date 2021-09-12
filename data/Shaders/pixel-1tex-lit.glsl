varying vec2 v_uv1;
varying float v_nDotL;

uniform sampler2D Texture0;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    color.xyz *= v_nDotL;

    gl_FragColor = color; 
}
