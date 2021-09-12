varying vec2 v_uv1;

uniform sampler2D Texture0;
uniform vec4 EntColor;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    if (color.w < 0.5) discard;
    color.xyz *= EntColor.xyz;
    gl_FragColor = color; 
}
