#include "ghdefines.hlsl"

Texture2D Texture0 : register(t0);
SamplerState Texture0Sampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

cbuffer PerFrameBuffer : register(register_perframe)
{
	float AlphaTestVal : ALPHATESTVAL;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = Texture0.Sample(Texture0Sampler, input.uv);
	clip(texelColor.w - AlphaTestVal);
	return texelColor;
}
