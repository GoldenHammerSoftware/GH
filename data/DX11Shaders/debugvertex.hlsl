cbuffer PerFrameBuffer : register(b1)
{
	float4x4 ModelViewProj;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float4 diffuse : DIFFUSE;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 diffuse : DIFFUSE;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput vertexShaderOutput;

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(ModelViewProj, pos);
	vertexShaderOutput.pos = pos;

	vertexShaderOutput.diffuse = input.diffuse;

	return vertexShaderOutput;
}
