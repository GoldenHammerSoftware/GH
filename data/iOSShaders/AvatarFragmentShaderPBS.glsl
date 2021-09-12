varying highp vec2 vertexUV;
uniform sampler2D albedoTexture;

void main()
{
	highp vec4 color = texture2D( albedoTexture, vertexUV );
	gl_FragColor = color;
}
