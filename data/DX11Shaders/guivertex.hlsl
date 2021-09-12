#include "guiposition.hlsl"

struct VertexShaderInput
{
	float4 pos : POSITION;
	float2 uv : UV;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput vertexShaderOutput;

	vertexShaderOutput.pos = positionGUIVertex(input.pos.xy);
	vertexShaderOutput.uv = input.uv;

	return vertexShaderOutput;
}

