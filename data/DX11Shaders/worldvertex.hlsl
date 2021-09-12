cbuffer PerFrameBuffer : register( b1 )
{
    float4x4 ModelViewProj;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
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

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(ModelViewProj, pos);
	vertexShaderOutput.pos = pos;

	vertexShaderOutput.uv.x = input.uv.x;
	vertexShaderOutput.uv.y = input.uv.y;

    return vertexShaderOutput;
}
