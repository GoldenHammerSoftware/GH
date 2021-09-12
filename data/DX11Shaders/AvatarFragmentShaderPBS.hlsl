Texture2D albedoTexture : register(t0);
SamplerState albedoSampler : register(s0);

Texture2D surfaceTexture : register(t1);
SamplerState surfaceSampler : register(s1);

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

float4 main(PixelShaderInput input) : SV_TARGET
{
	return albedoTexture.Sample(albedoSampler, input.vertexUV);
	//return surfaceTexture.Sample(surfaceSampler, input.vertexUV);
}


//#version 330 core
//
//in vec2 vertexUV;
//out vec4 fragmentColor;
//
//uniform sampler2D albedo;
//uniform sampler2D surface;
//
//void main() {
//	vec4 color = texture2D(albedo, vertexUV);
//	fragmentColor = color;
//}