varying vec2 v_uv1;

uniform sampler2D Texture0;
uniform float AlphaTestVal;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    if (AlphaTestVal > color.w) {
        discard;
    }
    gl_FragColor = color; 
}
