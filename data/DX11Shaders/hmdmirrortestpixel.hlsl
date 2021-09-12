Texture2D HMDMirrorTexture : register(t0);
SamplerState HMDMirrorTextureSampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2 uv = input.uv;
	uv.x = clamp(uv.x, 0.0, 1.0);
	uv.y = clamp(uv.y, 0.0, 1.0);

	float4 texelColor = HMDMirrorTexture.Sample(HMDMirrorTextureSampler, uv);
	texelColor.w = 1.0;
	return texelColor;
}
