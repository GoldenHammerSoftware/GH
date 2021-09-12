varying lowp vec2 v_uv1;
varying lowp float v_nDotL;

uniform sampler2D Texture0;

void main()
{
    lowp vec4 color = texture2D(Texture0, v_uv1);
    color.xyz *= v_nDotL;

    gl_FragColor = color; 
}
