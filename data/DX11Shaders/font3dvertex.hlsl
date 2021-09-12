#include "ghdefines.hlsl"

cbuffer PerTransBuffer : register(register_pertrans)
{
    float4x4 ModelViewProj;
};

struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 diffuse : DIFFUSE;
	float4 specular : SPECULAR;
	float2 uv : UV;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 diffuse : DIFFUSE;
	float4 specular : SPECULAR;
	float2 uv : UV;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

	vertexShaderOutput.pos = mul(ModelViewProj, input.pos);

	vertexShaderOutput.uv = input.uv;
	vertexShaderOutput.diffuse = input.diffuse;
	vertexShaderOutput.specular = input.specular;

    return vertexShaderOutput;
}
