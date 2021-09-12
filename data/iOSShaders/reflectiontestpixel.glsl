varying mediump vec2 v_uv1;

uniform sampler2D ReflectionTexture;

void main()
{
	gl_FragColor.xyz = texture2D(ReflectionTexture, v_uv1).xyz;
    gl_FragColor.w = 1.0; 
}
