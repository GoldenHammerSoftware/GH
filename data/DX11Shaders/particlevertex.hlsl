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
	float4 diffuse : DIFFUSE;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float4 diffuse : DIFFUSE;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(ModelViewProj, pos);
	vertexShaderOutput.pos = pos;

	vertexShaderOutput.uv = input.uv;
	vertexShaderOutput.normal = input.normal;
	vertexShaderOutput.diffuse = input.diffuse;

    return vertexShaderOutput;
}
