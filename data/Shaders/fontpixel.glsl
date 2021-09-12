varying vec2 v_uv1;
varying vec4 v_diffuse;
varying vec4 v_specular;

uniform sampler2D Texture0;

void main()
{
    vec4 outColor = texture2D(Texture0, v_uv1);
    if (outColor.w != 0.0)
    {
        outColor.xyz /= outColor.w;
        outColor.xyz = (v_diffuse.xyz*outColor.x)+(v_specular.xyz*outColor.y);
    }
    gl_FragColor = outColor;
}
