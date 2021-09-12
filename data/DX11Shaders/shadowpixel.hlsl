struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	// can't read from pos, so duplicated val.
	float4 usePos : USEPOS;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.usePos.z / input.usePos.w, 0.0, 0.0, 1.0);
}
