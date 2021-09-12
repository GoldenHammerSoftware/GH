varying vec2 v_uv1;
varying vec4 v_color;

uniform sampler2D Texture0;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    color.xyz /= (color.w+0.001);
    color *= v_color;
    gl_FragColor = color;
}
