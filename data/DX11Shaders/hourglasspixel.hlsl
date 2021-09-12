Texture2D simpleTexture : register(t0);
SamplerState simpleSampler : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2 uv = clamp(input.uv, 0.0, 1.0);
	float4 texelColor = simpleTexture.Sample(simpleSampler, uv);
    return texelColor;
}
