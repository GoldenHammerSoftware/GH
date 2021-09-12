varying mediump vec2 v_uv;

uniform sampler2D DiffuseTexture;

// low cost pixel shader for the reflection pass.
void main()
{
	mediump vec4 texelColor = texture2D(DiffuseTexture, v_uv);
    gl_FragColor = texelColor;
}

#if 0
varying mediump vec2 v_uv;
varying mediump vec2 v_uvlightmap;
varying mediump vec2 v_uvdetail;

uniform sampler2D DiffuseTexture;
uniform sampler2D DetailTexture;
uniform sampler2D LightmapTexture;

// low cost pixel shader for the reflection pass.
void main()
{
	mediump float lightAmbient = 0.4;

	mediump vec4 texelColor = texture2D(DiffuseTexture, v_uv);
	mediump vec4 detailColor = texture2D(DetailTexture, v_uvdetail);
	texelColor.xyz = mix(texelColor.xyz, detailColor.xyz, detailColor.w);
	mediump vec4 lightmapColor = texture2D(LightmapTexture, v_uvlightmap);
	lightmapColor = clamp(lightmapColor + lightAmbient, vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
	texelColor.xyz *= lightmapColor.xyz;
	texelColor.w = 1.0;

    gl_FragColor = texelColor;
}
#endif