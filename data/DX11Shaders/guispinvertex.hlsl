cbuffer PerFrameBuffer : register(b0)
{
	float Time;
	float GuiSpinSpeed;
};

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
	const float4x4 orthoProj = float4x4(	2, 0, 0, -1,
											0, -2, 0, 1,
											0, 0, -1, 0,
											0, 0, 0, 1);

	PixelShaderInput vertexShaderOutput;
	vertexShaderOutput.pos = mul(orthoProj, input.pos);

	float sinT = sin(Time*GuiSpinSpeed);
	float cosT = cos(Time*GuiSpinSpeed);
	vertexShaderOutput.uv.x = (input.uv.x - 0.5)*cosT + (input.uv.y - 0.5)*sinT + 0.5;
	vertexShaderOutput.uv.y = -(input.uv.x - 0.5)*sinT + (input.uv.y - 0.5)*cosT + 0.5;

    return vertexShaderOutput;
}
