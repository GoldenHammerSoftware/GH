varying vec2 v_uv1;

uniform sampler2D Texture0;

void main()
{
    vec2 uv = clamp(v_uv1, 0.0, 1.0);
    vec4 color = texture2D(Texture0, uv);
    gl_FragColor = color; 
}
