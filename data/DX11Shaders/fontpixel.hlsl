Texture2D Texture0 : register(t0);
SamplerState Texture0Sampler : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float4 diffuse : DIFFUSE;
	float4 specular : SPECULAR;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = Texture0.Sample(Texture0Sampler, input.uv);
	texelColor.xyz = (input.diffuse.xyz*texelColor.x) + (input.specular.xyz*texelColor.y);
	texelColor.w = 1;
	texelColor.x = 0;
	texelColor.y = 0;
	texelColor.z = 1;
    return texelColor;
}
