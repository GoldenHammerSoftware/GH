varying vec2 v_uv1;
varying vec2 v_uv2;
varying float v_nDotL;
varying vec2 v_ReflectionUV;
varying vec4 v_ReflectionPos;
varying float v_specular;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D ReflectionTexture;
uniform float ReflectionAmt;
uniform float LightMapScale;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    vec4 color2 = texture2D(Texture1, v_uv2);
    vec4 lightmap = texture2D(Texture2, v_uv1);

    vec2 reflectionUV;
    reflectionUV.x = (((v_ReflectionPos.x / v_ReflectionPos.w) * 0.5) + 0.5);
	reflectionUV.y = (((v_ReflectionPos.y / v_ReflectionPos.w) * 0.5) + 0.5);

    vec4 reflectionColor = texture2D(ReflectionTexture, reflectionUV);
    
    color.xyz = mix(color.xyz, color2.xyz, color2.w);
    color.xyz *= v_nDotL;
    float lightMapApply = lightmap.x*LightMapScale + 1.0-LightMapScale;
    color.xyz *= lightMapApply;
    color.xyz = mix(color.xyz, reflectionColor.xyz, ReflectionAmt);
    
    gl_FragColor = color; 
    gl_FragColor.xyz += v_specular;
}
