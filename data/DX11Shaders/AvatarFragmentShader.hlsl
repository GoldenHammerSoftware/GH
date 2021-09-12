
//hlsl port of Oculus Avatar sample shader

#define SAMPLE_MODE_COLOR 0
#define SAMPLE_MODE_TEXTURE 1
#define SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL 2
#define SAMPLE_MODE_PARALLAX 3
#define SAMPLE_MODE_RSRM 4

#define MASK_TYPE_NONE 0
#define MASK_TYPE_POSITIONAL 1
#define MASK_TYPE_REFLECTION 2
#define MASK_TYPE_FRESNEL 3
#define MASK_TYPE_PULSE 4

#define BLEND_MODE_ADD 0
#define BLEND_MODE_MULTIPLY 1

#define MAX_LAYER_COUNT 8

#include "ghdefines.hlsl"

Texture2D alphaMaskTexture : register(t0);
SamplerState alphaMaskSampler : register(s0);

Texture2D normalMapTexture : register(t1);
SamplerState normalMapSampler : register(s1);

Texture2D parallaxMapTexture : register(t2);
SamplerState parallaxMapSampler : register(s2);

Texture2D roughnessMapTexture : register(t3);
SamplerState roughnessMapSampler : register(s3);

Texture2D layerSurfaceTextures[MAX_LAYER_COUNT] : register(t4);
SamplerState layerSurfaceSamplers[MAX_LAYER_COUNT] : register(s4);

//note: each element of an array must start on a 4-float boundary
cbuffer PerFrameBuffer : register(register_perframe)
{
	float4 baseColor;
	float4 baseMaskParameters;
	float4 baseMaskAxis;
	float4 alphaMaskScaleOffset;
	float4 normalMapScaleOffset;
	float4 parallaxMapScaleOffset;
	float4 roughnessMapScaleOffset;
	float4x4 projectorInv;		//44 floats so far
	int layerSamplerModes[MAX_LAYER_COUNT]; //each of these will be the same as an array of float4, so 32 floats each
	int layerBlendModes[MAX_LAYER_COUNT];
	int layerMaskTypes[MAX_LAYER_COUNT];
	float4 layerColors[MAX_LAYER_COUNT];
	float4 layerSurfaceScaleOffsets[MAX_LAYER_COUNT];
	float4 layerSampleParameters[MAX_LAYER_COUNT];
	float4 layerMaskParameters[MAX_LAYER_COUNT];
	float4 layerMaskAxes[MAX_LAYER_COUNT];	//256 floats for all of the arrays, or 300 so far
	int layerCount;
	float Time; //renamed from elapsedSeconds
	int baseMaskType;
	float useAlpha;
	float useNormalMap;
	float useRoughnessMap;
	float useProjector;
}; //307 floats, or 1228 bytes for the whole cbuffer. Padded to nearest multiple of 4 floats, or 1232

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 vertexWorldPos : WORLDPOS;
	float3 vertexViewDir : VIEWDIR;
	float3 vertexObjPos : POSITION;
	float3 vertexNormal : NORMAL;
	float3 vertexTangent : TANGENT;
	float3 vertexBitangent : BITANGENT;
	float2 vertexUV : UV;
};

float3 ComputeNormal(float3x3 tangentTransform, float3 worldNormal, float3 surfaceNormal, float surfaceStrength)
{
	if (useNormalMap != 0)
	{
		float3 surface = lerp(float3(0.0, 0.0, 1.0), surfaceNormal, surfaceStrength);
		return normalize(mul(tangentTransform, surface));
	}
	else
	{
		return worldNormal;
	}
}

float3 ComputeColor(int sampleMode, float2 uv, float4 color, Texture2D surfaceTex, SamplerState surfaceSampler, float4 surfaceScaleOffset, float4 sampleParameters, float3x3 tangentTransform, float3 worldNormal, float3 surfaceNormal, float3 vertexViewDir)
{
	if (sampleMode == SAMPLE_MODE_TEXTURE)
	{
		float2 panning = Time * sampleParameters.xy;
		return surfaceTex.Sample(surfaceSampler, (uv + panning) * surfaceScaleOffset.xy + surfaceScaleOffset.zw).rgb * color.rgb;
	}
	else if (sampleMode == SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL)
	{
		float4 channelMask = sampleParameters;
		float4 channels = surfaceTex.Sample(surfaceSampler, uv * surfaceScaleOffset.xy + surfaceScaleOffset.zw);
		return dot(channelMask, channels) * color.rgb;
	}
	else if (sampleMode == SAMPLE_MODE_PARALLAX)
	{
		float parallaxMinHeight = sampleParameters.x;
		float parallaxMaxHeight = sampleParameters.y;
		float parallaxValue = parallaxMapTexture.Sample(parallaxMapSampler, uv * parallaxMapScaleOffset.xy + parallaxMapScaleOffset.zw).r;
		float scaledHeight = lerp(parallaxMinHeight, parallaxMaxHeight, parallaxValue);
		float2 parallaxUV = mul(tangentTransform, vertexViewDir).xy * scaledHeight;
		return surfaceTex.Sample(surfaceSampler, (uv + parallaxUV) * surfaceScaleOffset.xy + surfaceScaleOffset.zw).rgb * color.rgb;
	}
	else if (sampleMode == SAMPLE_MODE_RSRM)
	{
		float roughnessMin = sampleParameters.x;
		float roughnessMax = sampleParameters.y;
	
		float scaledRoughness = roughnessMin;
		if (useRoughnessMap != 0)
		{
			float roughnessValue = roughnessMapTexture.Sample(roughnessMapSampler, uv * roughnessMapScaleOffset.xy + roughnessMapScaleOffset.zw).r;
			scaledRoughness = lerp(roughnessMin, roughnessMax, roughnessValue);
		}
	
		float normalMapStrength = sampleParameters.z;
		float3 viewReflect = reflect(-vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength));
		float viewAngle = viewReflect.y * 0.5 + 0.5;
		return surfaceTex.Sample(surfaceSampler, float2(scaledRoughness, viewAngle)).rgb * color.rgb;
	}
	return color.rgb;
}

float ComputeMask(int maskType, float4 maskParameters, float4 maskAxis, float3x3 tangentTransform, float3 worldNormal, float3 surfaceNormal, float3 vertexObjPos, float3 vertexViewDir)
{
	if (maskType == MASK_TYPE_POSITIONAL) {
		float centerDistance = maskParameters.x;
		float fadeAbove = maskParameters.y;
		float fadeBelow = maskParameters.z;
		float d = dot(vertexObjPos, maskAxis.xyz);
		if (d > centerDistance) {
			return clamp(1.0 - (d - centerDistance) / fadeAbove, 0.0, 1.0);
		}
		else {
			return clamp(1.0 - (centerDistance - d) / fadeBelow, 0.0, 1.0);
		}
	}
	else if (maskType == MASK_TYPE_REFLECTION) {
		float fadeStart = maskParameters.x;
		float fadeEnd = maskParameters.y;
		float normalMapStrength = maskParameters.z;
		float3 viewReflect = reflect(-vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength));
		float d = dot(viewReflect, maskAxis.xyz);
		return clamp(1.0 - (d - fadeStart) / (fadeEnd - fadeStart), 0.0, 1.0);
	}
	else if (maskType == MASK_TYPE_FRESNEL) {
		float power = maskParameters.x;
		float fadeStart = maskParameters.y;
		float fadeEnd = maskParameters.z;
		float normalMapStrength = maskParameters.w;
		float d = 1.0 - max(0.0, dot(vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength)));
		float p = pow(d, power);
		return clamp(lerp(fadeStart, fadeEnd, p), 0.0, 1.0);
	}
	else if (maskType == MASK_TYPE_PULSE) {
		float distance = maskParameters.x;
		float speed = maskParameters.y;
		float power = maskParameters.z;
		float d = dot(vertexObjPos, maskAxis.xyz);
		float theta = 6.2831 * frac((d - Time * speed) / distance);
		return clamp(pow((sin(theta) * 0.5 + 0.5), power), 0.0, 1.0);
	}
	else {
		return 1.0;
	}
	return 1.0;
}

float3 ComputeBlend(int blendMode, float3 dst, float3 src, float mask)
{
	if (blendMode == BLEND_MODE_MULTIPLY)
	{
		return dst * (src * mask);
	}
	else {
		return dst + src * mask;
	}
}

float4 main(PixelShaderInput input) : SV_TARGET 
{
	float3 worldNormal = normalize(input.vertexNormal);
	float3x3 tangentTransform = float3x3(input.vertexTangent, input.vertexBitangent, worldNormal);

	float2 uv = input.vertexUV;
	if (useProjector != 0)
	{
		float4 projectorPos = mul(projectorInv, float4(input.vertexWorldPos, 1.0));
		if (abs(projectorPos.x) > 1.0 || abs(projectorPos.y) > 1.0 || abs(projectorPos.z) > 1.0)
		{
			discard;
		}
		uv = projectorPos.xy * 0.5 + 0.5;
	}

	float3 surfaceNormal = float3(0.0, 0.0, 1.0);
	if (useNormalMap != 0)
	{
		surfaceNormal.xy = normalMapTexture.Sample(normalMapSampler, uv * normalMapScaleOffset.xy + normalMapScaleOffset.zw).xy * 2.0 - 1.0;
		surfaceNormal.z = sqrt(1.0 - dot(surfaceNormal.xy, surfaceNormal.xy));
	}

	float4 color = baseColor;
	for (int i = 0; i < layerCount; ++i)
	{
		float3 layerColor = ComputeColor(layerSamplerModes[i], uv, layerColors[i], layerSurfaceTextures[i], layerSurfaceSamplers[i], layerSurfaceScaleOffsets[i], layerSampleParameters[i], tangentTransform, worldNormal, surfaceNormal, input.vertexViewDir);
		float layerMask = ComputeMask(layerMaskTypes[i], layerMaskParameters[i], layerMaskAxes[i], tangentTransform, worldNormal, surfaceNormal, input.vertexObjPos, input.vertexViewDir);
		color.rgb = ComputeBlend(layerBlendModes[i], color.rgb, layerColor, layerMask);
	}

	if (useAlpha != 0)
	{
		color.a *= alphaMaskTexture.Sample(alphaMaskSampler, uv * alphaMaskScaleOffset.xy + alphaMaskScaleOffset.zw).r;
	}
	return color;
}





//#version 330 core
//
//#define SAMPLE_MODE_COLOR 0
//#define SAMPLE_MODE_TEXTURE 1
//#define SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL 2
//#define SAMPLE_MODE_PARALLAX 3
//#define SAMPLE_MODE_RSRM 4
//
//#define MASK_TYPE_NONE 0
//#define MASK_TYPE_POSITIONAL 1
//#define MASK_TYPE_REFLECTION 2
//#define MASK_TYPE_FRESNEL 3
//#define MASK_TYPE_PULSE 4
//
//#define BLEND_MODE_ADD 0
//#define BLEND_MODE_MULTIPLY 1
//
//#define MAX_LAYER_COUNT 8
//
//in vec3 vertexWorldPos;
//in vec3 vertexViewDir;
//in vec3 vertexObjPos;
//in vec3 vertexNormal;
//in vec3 vertexTangent;
//in vec3 vertexBitangent;
//in vec2 vertexUV;
//out vec4 fragmentColor;
//
//uniform vec4 baseColor;
//uniform int baseMaskType;
//uniform vec4 baseMaskParameters;
//uniform vec4 baseMaskAxis;
//uniform sampler2D alphaMask;
//uniform vec4 alphaMaskScaleOffset;
//uniform sampler2D normalMap;
//uniform vec4 normalMapScaleOffset;
//uniform sampler2D parallaxMap;
//uniform vec4 parallaxMapScaleOffset;
//uniform sampler2D roughnessMap;
//uniform vec4 roughnessMapScaleOffset;
//
//uniform mat4 projectorInv;
//
//uniform bool useAlpha;
//uniform bool useNormalMap;
//uniform bool useRoughnessMap;
//uniform bool useProjector;
//uniform float elapsedSeconds;
//
//uniform int layerCount;
//
//uniform int layerSamplerModes[MAX_LAYER_COUNT];
//uniform int layerBlendModes[MAX_LAYER_COUNT];
//uniform int layerMaskTypes[MAX_LAYER_COUNT];
//uniform vec4 layerColors[MAX_LAYER_COUNT];
//uniform sampler2D layerSurfaces[MAX_LAYER_COUNT];
//uniform vec4 layerSurfaceScaleOffsets[MAX_LAYER_COUNT];
//uniform vec4 layerSampleParameters[MAX_LAYER_COUNT];
//uniform vec4 layerMaskParameters[MAX_LAYER_COUNT];
//uniform vec4 layerMaskAxes[MAX_LAYER_COUNT];
//
//vec3 ComputeNormal(mat3 tangentTransform, vec3 worldNormal, vec3 surfaceNormal, float surfaceStrength)
//{
//	if (useNormalMap)
//	{
//		vec3 surface = mix(vec3(0.0, 0.0, 1.0), surfaceNormal, surfaceStrength);
//		return normalize(surface * tangentTransform);
//	}
//	else
//	{
//		return worldNormal;
//	}
//}
//
//vec3 ComputeColor(int sampleMode, vec2 uv, vec4 color, sampler2D surface, vec4 surfaceScaleOffset, vec4 sampleParameters, mat3 tangentTransform, vec3 worldNormal, vec3 surfaceNormal)
//{
//	if (sampleMode == SAMPLE_MODE_TEXTURE)
//	{
//		vec2 panning = elapsedSeconds * sampleParameters.xy;
//		return texture(surface, (uv + panning) * surfaceScaleOffset.xy + surfaceScaleOffset.zw).rgb * color.rgb;
//	}
//	else if (sampleMode == SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL)
//	{
//		vec4 channelMask = sampleParameters;
//		vec4 channels = texture(surface, uv * surfaceScaleOffset.xy + surfaceScaleOffset.zw);
//		return dot(channelMask, channels) * color.rgb;
//	}
//	else if (sampleMode == SAMPLE_MODE_PARALLAX)
//	{
//		float parallaxMinHeight = sampleParameters.x;
//		float parallaxMaxHeight = sampleParameters.y;
//		float parallaxValue = texture(parallaxMap, uv * parallaxMapScaleOffset.xy + parallaxMapScaleOffset.zw).r;
//		float scaledHeight = mix(parallaxMinHeight, parallaxMaxHeight, parallaxValue);
//		vec2 parallaxUV = (vertexViewDir * tangentTransform).xy * scaledHeight;
//
//		return texture(surface, (uv + parallaxUV) * surfaceScaleOffset.xy + surfaceScaleOffset.zw).rgb * color.rgb;
//	}
//	else if (sampleMode == SAMPLE_MODE_RSRM)
//	{
//		float roughnessMin = sampleParameters.x;
//		float roughnessMax = sampleParameters.y;
//
//		float scaledRoughness = roughnessMin;
//		if (useRoughnessMap)
//		{
//			float roughnessValue = texture(roughnessMap, uv * roughnessMapScaleOffset.xy + roughnessMapScaleOffset.zw).r;
//			scaledRoughness = mix(roughnessMin, roughnessMax, roughnessValue);
//		}
//
//		float normalMapStrength = sampleParameters.z;
//		vec3 viewReflect = reflect(-vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength));
//		float viewAngle = viewReflect.y * 0.5 + 0.5;
//		return texture(surface, vec2(scaledRoughness, viewAngle)).rgb * color.rgb;
//	}
//	return color.rgb;
//}
//
//float ComputeMask(int maskType, vec4 maskParameters, vec4 maskAxis, mat3 tangentTransform, vec3 worldNormal, vec3 surfaceNormal)
//{
//	if (maskType == MASK_TYPE_POSITIONAL) {
//		float centerDistance = maskParameters.x;
//		float fadeAbove = maskParameters.y;
//		float fadeBelow = maskParameters.z;
//		float d = dot(vertexObjPos, maskAxis.xyz);
//		if (d > centerDistance) {
//			return clamp(1.0 - (d - centerDistance) / fadeAbove, 0.0, 1.0);
//		}
//		else {
//			return clamp(1.0 - (centerDistance - d) / fadeBelow, 0.0, 1.0);
//		}
//	}
//	else if (maskType == MASK_TYPE_REFLECTION) {
//		float fadeStart = maskParameters.x;
//		float fadeEnd = maskParameters.y;
//		float normalMapStrength = maskParameters.z;
//		vec3 viewReflect = reflect(-vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength));
//		float d = dot(viewReflect, maskAxis.xyz);
//		return clamp(1.0 - (d - fadeStart) / (fadeEnd - fadeStart), 0.0, 1.0);
//	}
//	else if (maskType == MASK_TYPE_FRESNEL) {
//		float power = maskParameters.x;
//		float fadeStart = maskParameters.y;
//		float fadeEnd = maskParameters.z;
//		float normalMapStrength = maskParameters.w;
//		float d = 1.0 - max(0.0, dot(vertexViewDir, ComputeNormal(tangentTransform, worldNormal, surfaceNormal, normalMapStrength)));
//		float p = pow(d, power);
//		return clamp(mix(fadeStart, fadeEnd, p), 0.0, 1.0);
//	}
//	else if (maskType == MASK_TYPE_PULSE) {
//		float distance = maskParameters.x;
//		float speed = maskParameters.y;
//		float power = maskParameters.z;
//		float d = dot(vertexObjPos, maskAxis.xyz);
//		float theta = 6.2831 * fract((d - elapsedSeconds * speed) / distance);
//		return clamp(pow((sin(theta) * 0.5 + 0.5), power), 0.0, 1.0);
//	}
//	else {
//		return 1.0;
//	}
//	return 1.0;
//}
//
//vec3 ComputeBlend(int blendMode, vec3 dst, vec3 src, float mask)
//{
//	if (blendMode == BLEND_MODE_MULTIPLY)
//	{
//		return dst * (src * mask);
//	}
//	else {
//		return dst + src * mask;
//	}
//}
//
//void main() {
//	vec3 worldNormal = normalize(vertexNormal);
//	mat3 tangentTransform = mat3(vertexTangent, vertexBitangent, worldNormal);
//
//	vec2 uv = vertexUV;
//	if (useProjector)
//	{
//		vec4 projectorPos = projectorInv * vec4(vertexWorldPos, 1.0);
//		if (abs(projectorPos.x) > 1.0 || abs(projectorPos.y) > 1.0 || abs(projectorPos.z) > 1.0)
//		{
//			discard;
//			return;
//		}
//		uv = projectorPos.xy * 0.5 + 0.5;
//	}
//
//	vec3 surfaceNormal = vec3(0.0, 0.0, 1.0);
//	if (useNormalMap)
//	{
//		surfaceNormal.xy = texture2D(normalMap, uv * normalMapScaleOffset.xy + normalMapScaleOffset.zw).xy * 2.0 - 1.0;
//		surfaceNormal.z = sqrt(1.0 - dot(surfaceNormal.xy, surfaceNormal.xy));
//	}
//
//	vec4 color = baseColor;
//	for (int i = 0; i < layerCount; ++i)
//	{
//		vec3 layerColor = ComputeColor(layerSamplerModes[i], uv, layerColors[i], layerSurfaces[i], layerSurfaceScaleOffsets[i], layerSampleParameters[i], tangentTransform, worldNormal, surfaceNormal);
//		float layerMask = ComputeMask(layerMaskTypes[i], layerMaskParameters[i], layerMaskAxes[i], tangentTransform, worldNormal, surfaceNormal);
//		color.rgb = ComputeBlend(layerBlendModes[i], color.rgb, layerColor, layerMask);
//	}
//
//	if (useAlpha)
//	{
//		color.a *= texture(alphaMask, uv * alphaMaskScaleOffset.xy + alphaMaskScaleOffset.zw).r;
//	}
//	color.a *= ComputeMask(baseMaskType, baseMaskParameters, baseMaskAxis, tangentTransform, worldNormal, surfaceNormal);
//	fragmentColor = color;
//}