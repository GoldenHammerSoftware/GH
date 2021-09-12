Texture2D Texture0 : register(t0);
SamplerState Texture0Sampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float4 diffuse : DIFFUSE;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = Texture0.Sample(Texture0Sampler, input.uv);
	//texelColor.xyz *= input.diffuse.xyz;
	//texelColor.w *= input.diffuse.w;
	//texelColor *= input.diffuse.w;
	texelColor *= input.diffuse;

    return texelColor;
}
