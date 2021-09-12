struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float4 diffuse : DIFFUSE;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = float4(input.diffuse.x, input.diffuse.y, input.diffuse.z, 1.0);
	return texelColor;
}
