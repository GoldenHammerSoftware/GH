#include "ghdefines.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float MaterialAmbient;
	float MaterialEmissive;
};

cbuffer PerTransBuffer : register(register_pertrans)
{
    float4x4 ModelViewProj;
	float3 LightDir0;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
	float nDotL : NDOTL;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(ModelViewProj, pos);
	vertexShaderOutput.pos = pos;

	vertexShaderOutput.uv = input.uv;

	float LightAmbientAmount = 0.25 * (1.0-MaterialAmbient) + MaterialEmissive;
    vertexShaderOutput.nDotL = max(dot(LightDir0, input.normal), 0.0);
	vertexShaderOutput.nDotL += LightAmbientAmount;
	vertexShaderOutput.nDotL = min(vertexShaderOutput.nDotL, 1.0);

    return vertexShaderOutput;
}
