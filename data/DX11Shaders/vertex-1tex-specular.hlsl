#include "ghdefines.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float MaterialAmbient;
	float SpecularPower;
	float SpecularAmount;
	float3 EyePos;
	float LightAmbientAmount;
};

cbuffer PerTransBuffer : register(register_pertrans)
{
    float4x4 ModelViewProj;
	float4x4 WorldToModel;
	float3 LightDir0;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 uv : UV;
	float nDotL : NDOTL;
	float specular : SPECULAR;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vsOut;

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(ModelViewProj, pos);
	vsOut.pos = pos;

	vsOut.uv = input.uv;

    float lightAmbientAmount = LightAmbientAmount * (1.0-MaterialAmbient);
	vsOut.nDotL = max(dot(LightDir0, input.normal), 0.0);
	vsOut.nDotL += lightAmbientAmount;
	vsOut.nDotL = min(vsOut.nDotL, 1.0);

    float4 eyePos4;
    eyePos4.xyz = EyePos.xyz;
    eyePos4.w = 1.0;
    float4 modelEyePos = mul(WorldToModel, eyePos4);

    float3 eyeDir = input.pos.xyz - modelEyePos.xyz;
    eyeDir = normalize(eyeDir);

    float3 eyeReflect = reflect(eyeDir, input.normal);
    vsOut.specular = pow(max(0.0, dot(eyeReflect, LightDir0)), SpecularPower);
    vsOut.specular *= SpecularAmount;

    return vsOut;
}
