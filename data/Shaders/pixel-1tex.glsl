varying vec2 v_uv1;

uniform sampler2D Texture0;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    gl_FragColor = color; 
}
