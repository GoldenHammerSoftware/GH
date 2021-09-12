varying vec4 v_diffuse;

void main()
{
    gl_FragColor.xyz = v_diffuse.xyz;
    gl_FragColor.w = 1.0;
}
