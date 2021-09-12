uniform sampler2D Texture0;

varying mediump vec3 v_normal;
varying mediump vec2 v_uv;
varying mediump vec4 v_diffuse;

void main()
{
    mediump vec4 texelColor = texture2D(Texture0, v_uv);
    texelColor *= v_diffuse;
    gl_FragColor = texelColor;
}
