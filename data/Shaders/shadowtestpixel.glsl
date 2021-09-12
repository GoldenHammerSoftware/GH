varying vec2 v_uv1;

uniform sampler2D ShadowTexture;

void main()
{
	gl_FragColor = texture2D(ShadowTexture, v_uv1); 
}
