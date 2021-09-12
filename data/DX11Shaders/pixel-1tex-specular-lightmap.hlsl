#include "ghdefines.hlsl"

Texture2D Texture0 : register(t0);
SamplerState Texture0Sampler : register(s0);
Texture2D Texture2 : register(t2);
SamplerState Texture2Sampler : register(s2);

cbuffer PerFrameBuffer : register(register_perframe)
{
	float LightMapScale : LIGHTMAPSCALE;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
	float nDotL : NDOTL;
	float specular : SPECULAR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = Texture0.Sample(Texture0Sampler, input.uv);
	texelColor.xyz *= input.nDotL;

	float4 lightmapColor = Texture2.Sample(Texture2Sampler, input.uv);
	float lightmapApply = lightmapColor.x * LightMapScale + 1.0 - LightMapScale;
	texelColor.xyz *= lightmapApply;

	texelColor.xyz += input.specular;
    return texelColor;
}
