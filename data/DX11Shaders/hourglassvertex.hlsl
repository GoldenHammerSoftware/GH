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

    vertexShaderOutput.pos = float4(input.pos.x*2.0f, input.pos.y*2.0f, 0.0f, 1.0f);
	vertexShaderOutput.pos.x -= 1.0f;
	vertexShaderOutput.pos.y -= 1.0f;
	vertexShaderOutput.pos.y *= -1.0f;

	vertexShaderOutput.uv.x = input.uv.x;
	vertexShaderOutput.uv.y = input.uv.y;

    return vertexShaderOutput;
}
