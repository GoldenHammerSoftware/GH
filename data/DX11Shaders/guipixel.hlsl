Texture2D simpleTexture : register(t0);
SamplerState simpleSampler : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = simpleTexture.Sample(simpleSampler, input.uv);
    return texelColor;
}
