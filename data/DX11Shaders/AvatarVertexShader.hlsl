//hlsl port of Oculus Avatar sample shader

#include "ghdefines.hlsl"

cbuffer PerFrameBuffer : register(register_perframe)
{
	float3 viewPos;
	float4x4 meshPose[64];
}

cbuffer PerTransBuffer : register(register_pertrans)
{
	float4x4 WorldToModel; //change: world -> WorldToModel (not sure if this is correct)
	float4x4 ModelViewProj; //change: viewProj -> ModelViewProj
}

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : UV;
	float4 poseIndices : BONEIDX;
	float4 poseWeights : BONEWEIGHT;
};

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

PixelShaderInput main(VertexShaderInput input)
{
    float4 vertexPose;
    vertexPose =  mul(meshPose[input.poseIndices.x*255], float4(input.position, 1.0)) * input.poseWeights.x;
    vertexPose += mul(meshPose[input.poseIndices.y*255], float4(input.position, 1.0)) * input.poseWeights.y;
    vertexPose += mul(meshPose[input.poseIndices.z*255], float4(input.position, 1.0)) * input.poseWeights.z;
    vertexPose += mul(meshPose[input.poseIndices.w*255], float4(input.position, 1.0)) * input.poseWeights.w;
    
    float4 normalPose;
    normalPose =  mul(meshPose[input.poseIndices.x*255], float4(input.normal, 0.0)) * input.poseWeights.x;
    normalPose += mul(meshPose[input.poseIndices.y*255], float4(input.normal, 0.0)) * input.poseWeights.y;
    normalPose += mul(meshPose[input.poseIndices.z*255], float4(input.normal, 0.0)) * input.poseWeights.z;
    normalPose += mul(meshPose[input.poseIndices.w*255], float4(input.normal, 0.0)) * input.poseWeights.w;
    normalPose = normalize(normalPose);

	float4 tangentPose;
    tangentPose =  mul(meshPose[input.poseIndices.x*255], float4(input.tangent.xyz, 0.0)) * input.poseWeights.x;
    tangentPose += mul(meshPose[input.poseIndices.y*255], float4(input.tangent.xyz, 0.0)) * input.poseWeights.y;
    tangentPose += mul(meshPose[input.poseIndices.z*255], float4(input.tangent.xyz, 0.0)) * input.poseWeights.z;
    tangentPose += mul(meshPose[input.poseIndices.w*255], float4(input.tangent.xyz, 0.0)) * input.poseWeights.w;
	tangentPose = normalize(tangentPose);

	PixelShaderInput vertexShaderOutput;

	vertexShaderOutput.vertexWorldPos = mul(WorldToModel, vertexPose).xyz; //change: world -> WorldToModel (not sure if this is correct - it's only used for the projector render)
	vertexShaderOutput.position = mul(ModelViewProj, vertexPose); //change: viewProj -> ModelViewProj
	vertexShaderOutput.vertexViewDir = normalize(viewPos - vertexShaderOutput.vertexWorldPos.xyz);
    vertexShaderOutput.vertexObjPos = input.position.xyz;
	vertexShaderOutput.vertexNormal = mul(WorldToModel, normalPose).xyz;
	vertexShaderOutput.vertexTangent = mul(WorldToModel, tangentPose).xyz;
	vertexShaderOutput.vertexBitangent = normalize(cross(vertexShaderOutput.vertexNormal, vertexShaderOutput.vertexTangent) * input.tangent.w);
	vertexShaderOutput.vertexUV = input.texCoord;

	return vertexShaderOutput;
}



//layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 normal;
//layout(location = 2) in vec4 tangent;
//layout(location = 3) in vec2 texCoord;
//layout(location = 4) in vec4 poseIndices;
//layout(location = 5) in vec4 poseWeights;
//out vec3 vertexWorldPos;
//out vec3 vertexViewDir;
//out vec3 vertexObjPos;
//out vec3 vertexNormal;
//out vec3 vertexTangent;
//out vec3 vertexBitangent;
//out vec2 vertexUV;
//uniform vec3 viewPos;
//uniform mat4 world;
//uniform mat4 viewProj;
//uniform mat4 meshPose[64];
//void main() {
//	vec4 vertexPose;
//	vertexPose = meshPose[int(poseIndices[0])] * float4(position, 1.0) * poseWeights[0];
//	vertexPose += meshPose[int(poseIndices[1])] * float4(position, 1.0) * poseWeights[1];
//	vertexPose += meshPose[int(poseIndices[2])] * float4(position, 1.0) * poseWeights[2];
//	vertexPose += meshPose[int(poseIndices[3])] * float4(position, 1.0) * poseWeights[3];
//
//	vec4 normalPose;
//	normalPose = meshPose[int(poseIndices[0])] * float4(normal, 0.0) * poseWeights[0];
//	normalPose += meshPose[int(poseIndices[1])] * float4(normal, 0.0) * poseWeights[1];
//	normalPose += meshPose[int(poseIndices[2])] * float4(normal, 0.0) * poseWeights[2];
//	normalPose += meshPose[int(poseIndices[3])] * float4(normal, 0.0) * poseWeights[3];
//	normalPose = normalize(normalPose);
//
//	vec4 tangentPose;
//	tangentPose = meshPose[int(poseIndices[0])] * float4(tangent.xyz, 0.0) * poseWeights[0];
//	tangentPose += meshPose[int(poseIndices[1])] * float4(tangent.xyz, 0.0) * poseWeights[1];
//	tangentPose += meshPose[int(poseIndices[2])] * float4(tangent.xyz, 0.0) * poseWeights[2];
//	tangentPose += meshPose[int(poseIndices[3])] * float4(tangent.xyz, 0.0) * poseWeights[3];
//	tangentPose = normalize(tangentPose);
//
//	vertexWorldPos = float3(world * vertexPose);
//	gl_Position = viewProj * float4(vertexWorldPos, 1.0);
//	vertexViewDir = normalize(viewPos - vertexWorldPos.xyz);
//	vertexObjPos = position.xyz;
//	vertexNormal = (world * normalPose).xyz;
//	vertexTangent = (world * tangentPose).xyz;
//	vertexBitangent = normalize(cross(vertexNormal, vertexTangent) * tangent.w);
//	vertexUV = texCoord;
//}