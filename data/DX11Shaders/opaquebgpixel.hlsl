#include "ghdefines.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float Opacity : OPACTIY;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = float4(0.5,0.5,0.5,Opacity);
    return color;
}
