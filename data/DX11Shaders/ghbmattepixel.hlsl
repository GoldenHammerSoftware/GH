#include "ghdefines.hlsl"

Texture2D Texture0 : register(t0);
SamplerState Texture0Sampler : register(s0);
Texture2D Texture1 : register(t1);
SamplerState Texture1Sampler : register(s1);

cbuffer PerFrameBuffer : register(register_perframe)
{
	float LightMapScale : LIGHTMAPSCALE;
	float3 Tint : TINT;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
	float2 uvlightmap: UVLIGHTMAP;
	float nDotL : NDOTL;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = Texture0.Sample(Texture0Sampler, input.uv);
	texelColor.xyz *= Tint;
	texelColor.xyz *= input.nDotL;

	float4 lightmapColor = Texture1.Sample(Texture1Sampler, input.uvlightmap);
	float lightmapApply = lightmapColor.x * LightMapScale + 1.0 - LightMapScale;
	texelColor.xyz *= lightmapApply;

    return texelColor;
}
