varying vec2 v_uv1;
varying vec2 v_uv2;
varying vec4 v_position;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D ShadowTexture;
uniform mat4 ShadowViewProj;

void main()
{
    vec4 color = texture2D(Texture0, v_uv1);
    vec4 color3 = texture2D(Texture2, v_uv1);
    color.xyz = mix(color.xyz, color3.xyz, color3.w);
    vec4 color2 = texture2D(Texture1, v_uv2);
    color.xyz = mix(color.xyz, color2.xyz, color2.w);
    
    vec2 v_shadowUV;
    float v_shadowBlend;
    vec4 shadowProjPos = ShadowViewProj * v_position;
	v_shadowUV.x = (((shadowProjPos.x / shadowProjPos.w) * 0.5) + 0.5);
	v_shadowUV.y = (((shadowProjPos.y / shadowProjPos.w) * 0.5) + 0.5);
	v_shadowUV = clamp(v_shadowUV, vec2(0.0,0.0), vec2(1.0,1.0));
	
	vec2 distFromCenter = abs(vec2(0.5,0.5) - v_shadowUV);
	float maxDistFromCenter = max(distFromCenter.x, distFromCenter.y);
	v_shadowBlend = 0.5 - maxDistFromCenter;
	v_shadowBlend = min(1.0, v_shadowBlend*10.0);

    vec4 shadowColor = texture2D(ShadowTexture, v_shadowUV);
    float shadowSub = shadowColor.w * v_shadowBlend * 0.25;
	color.xyz -= shadowSub;
    
    gl_FragColor = color; 
}
