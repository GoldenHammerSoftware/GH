varying lowp vec2 v_uv1;

uniform sampler2D Texture0;

void main()
{
    lowp vec2 uv = clamp(v_uv1, 0.0, 1.0);
    lowp vec4 color = texture2D(Texture0, uv);
    gl_FragColor = color; 
}
