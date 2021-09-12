// default fallback metal shaders.
#include <metal_stdlib>
using namespace metal;

#include "GHMetal/GHMetalShaderStructs.h"

struct PerTransBuffer
{
    float4x4 ModelViewProj;
};

struct GUIPerEntBuffer
{
    float4x4 GUIInWorldTransform;
};

struct VertexIn
{
    float3 position [[attribute(GHMTL_AI_POS)]];
    float2 uv [[attribute(GHMTL_AI_UV0)]];
};

struct VertexOut
{
    float4 position [[position]];
    float2 uv;
};

//vertex VertexOut vertex_default(VertexIn vertexIn [[stage_in]])
vertex VertexOut vertex_default( VertexIn vertexIn [[stage_in]],
                                const device PerTransBuffer& perTransBuffer [[buffer(GHMTL_BI_VPERTRANS)]])
{
    VertexOut vertexOut;
    vertexOut.position = float4(vertexIn.position, 1);
    vertexOut.position = perTransBuffer.ModelViewProj * vertexOut.position;
    vertexOut.uv = vertexIn.uv;
    return vertexOut;
}

vertex VertexOut vertex_gui_default( VertexIn vertexIn [[stage_in]],
                                    const device GUIPerEntBuffer& perEntBuffer [[buffer(GHMTL_BI_VPERENT)]]) {
    VertexOut vertexOut;
    vertexOut.position = float4(vertexIn.position, 1);
//    vertexOut.position.xy *= 2.0;
//    vertexOut.position.xy -= 1.0;
    vertexOut.position = perEntBuffer.GUIInWorldTransform * vertexOut.position;
    vertexOut.uv = vertexIn.uv;
    return vertexOut;
}

fragment float4 fragment_default(VertexOut fragmentIn [[stage_in]],
                                 texture2d<float> Texture0 [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    const float4 colorSample = Texture0.sample(textureSampler, fragmentIn.uv);
    return colorSample;
    
    //return float4(fragmentIn.uv[0], fragmentIn.uv[1], 0, 1);
}

