#include "ghdefines.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float4 TintColor : TINTCOLOR;
};

struct PixelShaderInput
{
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return TintColor;
}
