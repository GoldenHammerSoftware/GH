varying lowp vec2 v_uv1;
varying lowp float v_nDotL;
varying lowp float v_specular;

uniform sampler2D Texture0;
uniform sampler2D Texture2;
uniform lowp float LightMapScale;

void main()
{
    lowp vec4 color = texture2D(Texture0, v_uv1);
    lowp vec4 lightmap = texture2D(Texture2, v_uv1);

    color.xyz *= v_nDotL;
    lowp float lightMapApply = lightmap.x*LightMapScale + 1.0-LightMapScale;
    color.xyz *= lightMapApply;
    
    gl_FragColor = color; 
    gl_FragColor.xyz += v_specular;
}
