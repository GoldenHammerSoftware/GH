#include "ghdefines.hlsl"

cbuffer PerTransBuffer : register(register_pertrans)
{
	float4x4 ModelViewProj;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	// can't read from pos, so duplicated val.
	float4 usePos : USEPOS;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput vsOut;
	vsOut.pos = mul(ModelViewProj, float4(input.pos, 1.0));
	vsOut.usePos = vsOut.pos;
	return vsOut;
}
