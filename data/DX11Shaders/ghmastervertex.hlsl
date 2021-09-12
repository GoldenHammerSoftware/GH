#include "ghdefines.hlsl"
#include "ghlighting.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float4x4 ViewProj;
#ifdef SHADER_SUPPORTS_REFLECTION
	float4x4 ReflectionViewProj;
#endif
#ifdef SHADER_SUPPORTS_SHADOW
	float4x4 ShadowViewProj;
#endif
	float2 DiffuseUVScale;
	float2 DetailUVScale;
	float PBRUsesDetailUV;
};

cbuffer PerTransBuffer : register(register_pertrans)
{
	float4x4 ModelToWorld;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
	float3 normal : NORMAL;
#ifdef SHADER_SUPPORTS_BUMPMAP
	float3 tangent : TANGENT;
#endif
	float2 uv : UV;
#if defined(SHADER_SUPPORTS_LIGHTMAP) || defined(SHADER_DETAIL_UV_CHANNEL)
	float2 uv2 : UVTWO;
#endif
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
#ifdef SHADER_SUPPORTS_REFLECTION
	float4 reflectionPos : REFLECTIONPOS;
#endif
#ifdef SHADER_SUPPORTS_SHADOW
	float4 shadowPos : SHADOWPOS;
#endif
	float4 modelPos : MODELPOS;
	float4 worldPos : WORLDPOS;
	float3 normal : NORMAL;
#ifdef SHADER_SUPPORTS_BUMPMAP
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
#endif
	float2 uv : UV;
#ifdef SHADER_SUPPORTS_LIGHTMAP
	float2 uvlightmap : UV2;
#endif
	float2 uvdetail : UV3;
	float2 uvpbr : UV4;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

	float4 pos = float4(input.pos, 1.0f);
	float4 worldPos = mul(ModelToWorld, pos);
	vertexShaderOutput.pos = mul(ViewProj, worldPos);

	vertexShaderOutput.uv = input.uv * DiffuseUVScale;
#ifdef SHADER_SUPPORTS_LIGHTMAP
	vertexShaderOutput.uvlightmap = input.uv2;
#endif

#if defined(SHADER_DETAIL_UV_CHANNEL) && (SHADER_DETAIL_UV_CHANNEL == 1)
	vertexShaderOutput.uvdetail = input.uv2;
	float2 centerOffset = float2(0.5, 0.5);
	vertexShaderOutput.uvdetail -= centerOffset;
	vertexShaderOutput.uvdetail *= DetailUVScale;
	vertexShaderOutput.uvdetail += centerOffset;
#else
	vertexShaderOutput.uvdetail = input.uv * DetailUVScale;
#endif

	// decide whether the pbr textures will use the base or detail uv channel.
	vertexShaderOutput.uvpbr = lerp(vertexShaderOutput.uv, vertexShaderOutput.uvdetail, PBRUsesDetailUV);

#ifdef SHADER_SUPPORTS_REFLECTION
	vertexShaderOutput.reflectionPos = mul(ReflectionViewProj, worldPos);
	// the *2 is an optimization here so we don't do it on the pixel, see reflectioninclude
	vertexShaderOutput.reflectionPos.w *= 2.0;
#endif
#ifdef SHADER_SUPPORTS_SHADOW
	vertexShaderOutput.shadowPos = mul(ShadowViewProj, worldPos);
	// the *2 is an optimization here so we don't do it on the pixel, see reflectioninclude
	vertexShaderOutput.shadowPos.w *= 2.0;
#endif

	vertexShaderOutput.normal = input.normal;
	//calcTangentBinormal(input.normal, vertexShaderOutput.tangent, vertexShaderOutput.binormal);

	vertexShaderOutput.modelPos = pos;
	vertexShaderOutput.worldPos = worldPos;

#ifdef SHADER_SUPPORTS_BUMPMAP
	vertexShaderOutput.tangent = input.tangent;
	vertexShaderOutput.binormal = cross(input.normal, input.tangent);
#endif

	return vertexShaderOutput;
}
