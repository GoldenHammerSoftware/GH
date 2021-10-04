#import "GHMetalPipelineMgr.h"
#include "GHMetalRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHiOSViewMetal.h"
#include "GHMetalShaderStructs.h"
#include "GHMDesc.h"
#include "GHMetalVBBlitterIndex.h"
#include "GHMetalToGHConversions.h"
#include "GHShaderType.h"

GHMetalPipelineMgr::GHMetalPipelineMgr(GHMetalRenderDevice& device)
: mDevice(device)
{
    // Create a dummy vertex descriptor for initializing pipeline states.
    mDefaultVertexDescriptor = [MTLVertexDescriptor new];
    [mDefaultVertexDescriptor reset];
    for (int i = (int)GHMTL_AI_POS; i < (int)GHMTL_AI_MAX; ++i)
    {
        mDefaultVertexDescriptor.attributes[i].bufferIndex = 0;
        mDefaultVertexDescriptor.attributes[i].format = MTLVertexFormatFloat3;
        mDefaultVertexDescriptor.attributes[i].offset = 0;
    }
    mDefaultVertexDescriptor.layouts[0].stride = 3 * sizeof(float);
    mDefaultVertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    mDefaultVertexDescriptor.layouts[0].stepRate = 1;
    
    mPipelineDescriptor = getNewPipelineDescriptor();
}

GHMetalPipelineMgr::~GHMetalPipelineMgr(void)
{
    mDefaultVertexDescriptor = nil;
    mPipelineDescriptor = nil;
}

const GHMetalPipelineMgr::PipelineWrapper& GHMetalPipelineMgr::getPipelineState(const GHMDesc& desc, GHMetalVBBlitterIndex* blitter, id<MTLFunction> __strong (&shaders)[GHShaderType::ST_MAX])
{
    CacheIndex cacheId;
    cacheId.mBlitter = blitter;
    cacheId.mDesc = &desc;
    cacheId.mVertexShader = shaders[GHShaderType::ST_VERTEX];
    cacheId.mPixelShader = shaders[GHShaderType::ST_PIXEL];

    auto cacheFind = mPipelineCache.find(cacheId);
    if (cacheFind != mPipelineCache.end())
    {
        return cacheFind->second;
    }
    
    if (blitter)
    {
        mPipelineDescriptor.vertexDescriptor = blitter->getVertexDescriptor();
    }
    else
    {
        // Add a dummy vertex descriptor so we have something valid and
        //  can access reflection.
        mPipelineDescriptor.vertexDescriptor = mDefaultVertexDescriptor;
    }
    
    mPipelineDescriptor.colorAttachments[0].blendingEnabled = desc.mAlphaBlend ? YES : NO;
    if (desc.mAlphaBlend)
    {
        mPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = GHMetal::metalBlendForGHBlend(desc.mSrcBlend);
        mPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = GHMetal::metalBlendForGHBlend(desc.mDstBlend);
    }
    mPipelineDescriptor.vertexFunction = shaders[GHShaderType::ST_VERTEX];
    mPipelineDescriptor.fragmentFunction = shaders[GHShaderType::ST_PIXEL];
    
    // We don't have per-pass pipeline descriptors,
    // so MSAA has to apply to all targets atm.
    mPipelineDescriptor.sampleCount = mDevice.getMsaaSampleCount();
    
    mPipelineCache[cacheId] = createPipelineState(mPipelineDescriptor);
    // returning the cache copy in case the caller wants to keep it around.
    return mPipelineCache[cacheId];
}

MTLRenderPipelineDescriptor* GHMetalPipelineMgr::getNewPipelineDescriptor(void)
{
    // return a pipeline description with the common things set.
    MTLRenderPipelineDescriptor* ret = [MTLRenderPipelineDescriptor new];
    ret.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    ret.colorAttachments[0].blendingEnabled = NO;
    ret.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    ret.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    ret.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    ret.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    ret.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    ret.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    ret.depthAttachmentPixelFormat = (&mDevice.getiOSView()).depthStencilPixelFormat;
    
    // vertexFunction, fragmentFunction, vertexDescription must be
    //  populated before calling getPipelineState.

    return ret;
}

GHMetalPipelineMgr::PipelineWrapper GHMetalPipelineMgr::createPipelineState(MTLRenderPipelineDescriptor* desc)
{
    NSError *error = nil;
    MTLRenderPipelineReflection* reflectionObj;
    MTLPipelineOption option = MTLPipelineOptionBufferTypeInfo | MTLPipelineOptionArgumentInfo;
    id <MTLRenderPipelineState> pstate = [mDevice.getMetalDevice() newRenderPipelineStateWithDescriptor:desc options:option reflection:&reflectionObj error:&error];

    GHMetalPipelineMgr::PipelineWrapper ret;
    if (!pstate)
    {
        GHDebugMessage::outputString("Failed to make pipeline state");
        return ret;
    }
    ret.mPipelineState = pstate;
    ret.mReflection = reflectionObj;
    return ret;
}
