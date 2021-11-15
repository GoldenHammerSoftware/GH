#include "ghdefines.hlsl"
#include "reflectioninclude.hlsl"
#include "ghlighting.hlsl"

Texture2D DiffuseTexture : register(t0);
SamplerState DiffuseTextureSampler : register(s0);
Texture2D MetalTexture : register(t1);
SamplerState MetalTextureSampler : register(s1);

Texture2D DetailTexture : register(t2);
SamplerState DetailTextureSampler : register(s2);

#ifdef SHADER_SUPPORTS_LIGHTMAP
Texture2D LightmapTexture : register(t3);
SamplerState LightmapTextureSampler : register(s3);
#endif

#ifdef SHADER_SUPPORTS_REFLECTION
Texture2D ReflectionTexture: register(t4);
SamplerState ReflectionTextureSampler : register(s4);
#endif

#ifdef SHADER_SUPPORTS_BUMPMAP
Texture2D NormalTexture: register(t5);
SamplerState NormalTextureSampler : register(s5);
#endif

#ifdef SHADER_PBR_UE4
Texture2D SpecularTexture: register(t6);
SamplerState SpecularTextureSampler : register(s6);
#endif

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
Texture2D EmissiveTexture: register(t7);
SamplerState EmissiveTextureSampler : register(s7);
#endif 

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
TextureCube EnvironmentTexture: register(t8);
SamplerState EnvironmentTextureSampler : register(s8);
#endif

#ifdef SHADER_SUPPORTS_SHADOW
Texture2D ShadowTexture: register(t9);
SamplerState ShadowTextureSampler : register(s9);
#endif

cbuffer PerFrameBuffer : register(register_perframe)
{
#ifdef SHADER_SUPPORTS_LIGHTMAP
	float LightMapScale;
#endif
#ifdef SHADER_SUPPORTS_REFLECTION
	float ReflectionAmt;
#endif
	float MaterialAmbient; 
	float MaterialEmissive;
	float LightAmbientAmount;
#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
	float EnvironmentMapIntensity;
#endif
#ifdef SHADER_SUPPORTS_SHADOW
	float3 ShadowCenter;
	float ShadowRadius;
#endif
};

cbuffer PerTransBuffer : register(register_pertrans)
{
	float4x4 WorldToModel;
	float4x4 ModelToWorld;
	float3 EyePosModel;

	// todo: replace with array.
	float3 LightDir0;
	float3 LightIntensity0;
	float3 LightDir1;
	float3 LightIntensity1;
};

#if defined(SHADER_SUPPORTS_TINT) || defined(SHADER_SUPPORTS_ENT_COLOR_MASK)
cbuffer PerEntBuffer : register(register_perent)
{
	float4 EntColor;
	#ifdef SHADER_SUPPORTS_ENT_COLOR_MASK
		float4 EntColor2;
	#endif
};
#endif

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

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = DiffuseTexture.Sample(DiffuseTextureSampler, input.uv);
	return texelColor;

	float4 detailColor = DetailTexture.Sample(DetailTextureSampler, input.uvdetail);
	float4 metalColor = MetalTexture.Sample(MetalTextureSampler, input.uvpbr);

#ifdef SHADER_SUPPORTS_TINT
	texelColor *= EntColor;
#else 
	#ifdef SHADER_SUPPORTS_ENT_COLOR_MASK
		float4 entColorBlend = (texelColor.r * EntColor) + ((1.0 - texelColor.r) * EntColor2);
		texelColor = entColorBlend * (1.0 - texelColor.b);
	#endif
#endif

#ifdef SHADER_DETAIL_DARKEN
	texelColor.xyz *= lerp(detailColor.xyz, float3(1.0, 1.0, 1.0), (1.0 - detailColor.w));
#else
	texelColor.xyz = lerp(texelColor.xyz, detailColor.xyz, detailColor.w);
#endif

#ifdef SHADER_PBR_UE4
	float4 specularColor = SpecularTexture.Sample(SpecularTextureSampler, input.uvpbr);

	float occlusion = metalColor.r;
	float roughness = metalColor.g;
	float metallic = metalColor.b;
	float specularMult = specularColor.r;
#else
	float metallic = metalColor.r;
	float roughness = (1.0 - metalColor.g);
	float specularMult = metalColor.b;
	float occlusion = 1.0;
#endif

	// hack to avoid bad specular
	roughness = max(roughness, 0.4);

	float lightAmbient = LightAmbientAmount * MaterialAmbient;

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
#else
	lightAmbient = saturate(lightAmbient + MaterialEmissive);
#endif

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
	float3 viewVecModel = (input.modelPos.xyz - EyePosModel);
#endif

#ifdef SHADER_SUPPORTS_BUMPMAP
	float4 normalColor = NormalTexture.Sample(NormalTextureSampler, input.uv);
	//float3 bumpNormal = applyBumpmapNormal(normalColor.xyz, input.normal, viewVecModel, input.uv);
	float3 bumpNormal = applyBumpmapNormal(normalColor.xyz, input.normal, input.tangent, input.binormal);
#else
	float3 bumpNormal = input.normal;
#endif
	// possibly paranoia
	bumpNormal = normalize(bumpNormal);

	GHLight lights[kMaxLights];
	lights[0].mVec = LightDir0;
	lights[0].mIntensity = LightIntensity0;
#if (kMaxLights > 1)
	lights[1].mVec = LightDir1;
	lights[1].mIntensity = LightIntensity1;
#endif

	float lightIntensityMod = 1.0;
#if defined(SHADER_SUPPORTS_LIGHTMAP) || defined(SHADER_SUPPORTS_SHADOW)

#ifdef SHADER_SUPPORTS_LIGHTMAP
	float4 lightmapColor = LightmapTexture.Sample(LightmapTextureSampler, input.uvlightmap);
	lightmapColor.xyz = lerp(float3(1.0, 1.0, 1.0), lightmapColor.xyz, LightMapScale);
	lightIntensityMod *= lightmapColor.r;
	lightIntensityMod *= occlusion;
#endif // lightmap

#ifdef SHADER_SUPPORTS_SHADOW
	float shadowPct = getShadowAmount(input.shadowPos, ShadowTexture, ShadowTextureSampler, input.worldPos.xyz, ShadowCenter, ShadowRadius);
	lightIntensityMod *= (1.0 - shadowPct * 0.15);
#endif // shadow

#endif // lightmap/shadow

	lightAmbient *= occlusion;
	float3 eyeDir = normalize(EyePosModel - input.modelPos.xyz);

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)

// todo: move these somewhere better.
#ifdef GH_METAL
	const float diffuseEnvMip = 8.0;
	const float maxSpecularEnvMip = 3.0;
	const float minSpecularEnvMip = 0.0;
#else 
#ifdef GH_GLES
	// android
	const float diffuseEnvMip = 4.0;
	const float maxSpecularEnvMip = 3.0;
	const float minSpecularEnvMip = 0.0;
#else
	// pc most likely.
	const float diffuseEnvMip = 7;
	const float maxSpecularEnvMip = 6; //Arbitrary range here. The cubemap has 10 mip levels
	const float minSpecularEnvMip = 0;
#endif
#endif

	float3 diffuseEnvUVW = normalize(mul(ModelToWorld, float4(bumpNormal, 0.0)).xyz);
	diffuseEnvUVW[0] = -diffuseEnvUVW[0];
	float3 diffuseEnv = EnvironmentTexture.SampleLevel(EnvironmentTextureSampler, diffuseEnvUVW, diffuseEnvMip).xyz;

	float3 specEnvUVW = normalize(reflect(viewVecModel, bumpNormal));
	specEnvUVW = normalize(mul(ModelToWorld, float4(specEnvUVW, 0.0)).xyz);
	specEnvUVW[0] = -specEnvUVW[0];

	// One option for mip selection from http://casual-effects.blogspot.com/2011/08/plausible-environment-lighting-in-two.html:
	// m = log2(w*sqrt(3)) - 0.5 * log(s + 1)
	// cubeamp width is assumed to be 512, log2(w*sqrt(3)) = 9.79248125
	//float specularEnvMip = 10.0 - (9.79248125 - 0.5 * log2(roughness + 1));
	// ^^ this doesn't seem to give us much of a range in results, probably because roughness is the wrong unit (should be glossiness).
	// the subtraction from 10 is to invert the range.

	// Keeping with a logarithmic scale here: roughness is in range 0-1. log2(roughness+1) will also map to 0-1
	// Not sure if we want to give it a smaller range than that.
	// This formula is open to being tweaked if we don't like it -- I don't think it's that correct anyway.
	float specularEnvMip = minSpecularEnvMip + (maxSpecularEnvMip - minSpecularEnvMip) * log2(roughness + 1.0);
	float3 specularEnv = EnvironmentTexture.SampleLevel(EnvironmentTextureSampler, specEnvUVW, specularEnvMip).xyz;

#ifdef SHADER_SUPPORTS_ENVIRONMENTMAP
	// Make the reflection have less of an impact on diffuse.
	float3 diffuseReflection = lerp(float3(1.0, 1.0, 1.0), diffuseEnv, 0.1);
#else
	float3 diffuseReflection = lerp(float3(0.0, 0.0, 0.0), diffuseEnv, 0.5); //diffuseEnv;
#endif
	float3 specularReflection = specularEnv;

	float3 reflectionColor = cookTorranceOmniDirectional(texelColor.xyz, bumpNormal, metallic, roughness, specularMult,
		diffuseReflection, specularReflection, eyeDir);

#ifdef SHADER_SUPPORTS_ENVIRONMENTMAP
	texelColor.xyz = lerp(texelColor.xyz, reflectionColor, EnvironmentMapIntensity);
#endif // SHADER_SUPPORTS_ENVIRONMENTMAP

#endif // env or ibl

#ifdef SHADER_SUPPORTS_REFLECTION
//#ifdef SHADER_SUPPORTS_SHADOW
//	applyPixelReflection(input.reflectionPos, ReflectionTexture, ReflectionTextureSampler, ReflectionAmt * (1.0 - roughness) * lightIntensityMod, texelColor);
//#else // not shadow
	applyPixelReflection(input.reflectionPos, ReflectionTexture, ReflectionTextureSampler, ReflectionAmt * (1.0 - roughness), texelColor);
//#endif // shadow
#endif // reflection

	float4 litColor = texelColor;
	litColor.xyz = cookTorrance(texelColor.xyz, bumpNormal, metallic, roughness, specularMult, lights, lightAmbient, lightIntensityMod, eyeDir);

#ifdef SHADER_SUPPORTS_IBL
	litColor.xyz += reflectionColor;
#endif

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
	float4 emissiveColor = EmissiveTexture.Sample(EmissiveTextureSampler, input.uv);
	litColor.xyz = lerp(litColor.xyz, texelColor.xyz, emissiveColor.r + MaterialEmissive);
#endif

//#define DEBUG_NORMALS
#ifdef DEBUG_NORMALS
	litColor.xyz = normalize(mul(ModelToWorld, normalize(bumpNormal))) *0.5 + 0.5;
#endif

    return litColor;
}
