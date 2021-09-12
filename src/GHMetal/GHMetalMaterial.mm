#import "GHMetalMaterial.h"
#include "GHMDesc.h"
#include "GHMetalPipelineMgr.h"
#include "GHMetalShaderMgr.h"
#include "GHVertexBuffer.h"
#include "GHMetalVBBlitterIndex.h"
#include "GHMetalShaderStructs.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetalToGHConversions.h"
#include "GHMetalMaterialParamHandle.h"
#include "GHMetalMultiMaterialParamHandle.h"

#include "GHBillboardTransformAdjuster.h"
#include "GHMaterialCallback.h"

GHMetalMaterial::GHMetalMaterial(GHMetalRenderDevice& device, GHMetalPipelineMgr& pipelineMgr,
                                 GHMetalShaderMgr& shaderMgr, GHMDesc* desc)
: mDevice(device)
, mDesc(desc)
, mPipelineMgr(pipelineMgr)
, mShaderMgr(shaderMgr)
{
    for (int st = 0; st < ST_MAX; ++st)
    {
        mShaders[st] = nil;
    }
    
    mShaders[ST_VERTEX] = mShaderMgr.getShaderFunction(desc->mVertexFile);
    mShaders[ST_PIXEL] = mShaderMgr.getShaderFunction(desc->mPixelFile);
    initShaderReflection();
    
    mDescParamHandles = mDesc->initMaterial(*this);
    
    mDepthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
    if (mDesc->mZRead) mDepthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    else mDepthDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
    if (mDesc->mZWrite) mDepthDescriptor.depthWriteEnabled = true;
    else mDepthDescriptor.depthWriteEnabled = false;
    // todo: cache this between materials
    // possible todo: allow changing states between frames.  was crashing.
    mDepthStencilState = [mDevice.getMetalDevice() newDepthStencilStateWithDescriptor:mDepthDescriptor];
}

GHMetalMaterial::~GHMetalMaterial(void)
{
    // It probably isn't necessary to explicitly set metal objects to nil here.
    mPipelineState = nil;
    for (int st = 0; st < ST_MAX; ++st)
    {
        mShaders[st] = nil;
        for (size_t i = 0; i < GHMaterialCallbackType::CT_MAX; ++i)
        {
            free(mShaderBindings[st].mConstantBuffers[i].mData);
        }
    }
    
    delete mDesc;
    delete mDescParamHandles;
}

void GHMetalMaterial::beginMaterial(const GHViewInfo& viewInfo)
{
    mDevice.beginMaterial();
    
    // if the shader is shared between materials we need to
    // re-apply the default arguments that do not have callbacks.
    mDesc->applyDefaultArgs(*this, *mDescParamHandles);
    
    id<MTLRenderCommandEncoder> commandEncoder = mDevice.getRenderCommandEncoder();

    MTLWinding metalWinding = GHMetal::metalWindingForGHCull(mDesc->mCullMode);
    [commandEncoder setFrontFacingWinding:metalWinding];
    
    MTLCullMode metalCullMode = GHMetal::metalCullModeForGHCull(mDesc->mCullMode);
    [commandEncoder setCullMode:metalCullMode];
    
    MTLTriangleFillMode metalFillMode = MTLTriangleFillModeFill;
    if (mDesc->mWireframe) metalFillMode = MTLTriangleFillModeLines;
    // todo: hook up wireframe.
    
    [commandEncoder setDepthStencilState:mDepthStencilState];
    
    applyTextures();
    
    applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
    applyCBBuffer(GHMaterialCallbackType::CT_PERFRAME);
}

void GHMetalMaterial::beginVB(const GHVertexBuffer& vb)
{
    const GHVBBlitterPtr* vbBlitterPtr = vb.getBlitter();
    GHMetalVBBlitterIndex* ibBlitter = (GHMetalVBBlitterIndex*)(vbBlitterPtr->get());
    
    GHMetalPipelineMgr::PipelineWrapper wrapper = mPipelineMgr.getPipelineState(*mDesc, ibBlitter, mShaders);
    mPipelineState = wrapper.mPipelineState;
    
    [mDevice.getRenderCommandEncoder() setRenderPipelineState:mPipelineState];
}

void GHMetalMaterial::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(geoData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbVal);
    applyCBBuffer(GHMaterialCallbackType::CT_PERGEO);
}

void GHMetalMaterial::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(entData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbVal);
    applyCBBuffer(GHMaterialCallbackType::CT_PERENT);
}

void GHMetalMaterial::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
    GHBillboardTransformAdjuster transAdjuster;
    GHTransform scratchTransform;
    const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);
    
    GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
    applyCBBuffer(GHMaterialCallbackType::CT_PERTRANS);
}

void GHMetalMaterial::endMaterial(void)
{
}

GHMaterialParamHandle* GHMetalMaterial::getParamHandle(const char* paramName)
{
    const char* alias = mDesc->checkParamAlias(paramName);
    
    // find a matching param using string compare
    // todo: start using identifiers instead.
    std::vector<GHMaterialParamHandle*> params;
    for (size_t i = 0; i < mParamHandleDescs.size(); ++i)
    {
        if (mParamHandleDescs[i].mName == alias)
        {
            if (mParamHandleDescs[i].mIsTexture)
            {
                params.push_back(new GHMetalMaterialParamHandle(0, mParamHandleDescs[i].mOffset));
            }
            else
            {
                // todo: handle when the same param is sent to both pixel and vertex.
                params.push_back(new GHMetalMaterialParamHandle(mShaderBindings[mParamHandleDescs[i].mShaderType].mConstantBuffers[mParamHandleDescs[i].mCBType].mData, mParamHandleDescs[i].mOffset));
            }
        }
    }
    if (params.size() > 1)
    {
        GHMetalMultiMaterialParamHandle* ret = new GHMetalMultiMaterialParamHandle();
        for (size_t i = 0; i < params.size(); ++i)
        {
            ret->addSubHandle(params[i]);
        }
        return ret;
    }
    if (params.size() == 1)
    {
        return params[0];
    }
    return 0;
}

void GHMetalMaterial::applyTextures(void)
{
    mDesc->applyTextures(*this, *mDescParamHandles, false);
}

void GHMetalMaterial::initShaderReflection(void)
{
    // get the shader reflection as part of a pipeline creation.
    const GHMetalPipelineMgr::PipelineWrapper wrapper = mPipelineMgr.getPipelineState(*mDesc, 0, mShaders);
    
    if (!wrapper.mReflection)
    {
        GHDebugMessage::outputString("Failed to get shader reflection");
        return;
    }
    
    for (MTLArgument* arg in wrapper.mReflection.vertexArguments)
    {
        parseMTLArgument(arg, ST_VERTEX);
    }
    for (MTLArgument* arg in wrapper.mReflection.fragmentArguments)
    {
        parseMTLArgument(arg, ST_PIXEL);
    }
}

void GHMetalMaterial::parseMTLArgument(MTLArgument* arg, GHShaderType st)
{
    if (!(int)arg.active)
    {
        return;
    }

    int argIndex = (int)arg.index;
    int argType = (int)arg.type;
    bool argActive = (bool)arg.active;

    if (arg.type == MTLArgumentTypeTexture)
    {
        ParamHandleDesc hDesc;
        hDesc.mCBType = GHMaterialCallbackType::CT_PERFRAME;
        hDesc.mOffset = argIndex;
        hDesc.mName.setConstChars([arg.name UTF8String], GHString::CHT_COPY);
        hDesc.mIsTexture = true;
        mParamHandleDescs.push_back(hDesc);
    }
    else if (arg.type == MTLArgumentTypeSampler)
    {
        int brkpt = 1;
    }
    else if (arg.bufferDataType == MTLDataTypeStruct)
    {
        // this could be a constant buffer or the vb.
        // djw thinks that index 0 probably means a vb and not a const buffer.
        if (arg.index == 0)
        {
            return;
        }
        GHMaterialCallbackType::Enum cbType = GHMetal::getGHBufferIndex((GHMTL_BufferIndex)arg.index);
        size_t dataSize = arg.bufferDataSize;
        mShaderBindings[st].mConstantBuffers[cbType].mData = malloc(dataSize);
        mShaderBindings[st].mConstantBuffers[cbType].mDataSize = dataSize;
        assert(dataSize <= 4000 && "buffer too big for setVertexBytes");
        
        for (MTLStructMember* uniform in arg.bufferStructType.members)
        {
            ParamHandleDesc hDesc;
            hDesc.mCBType = cbType;
            hDesc.mOffset = uniform.offset;
            hDesc.mName.setConstChars([uniform.name UTF8String], GHString::CHT_COPY);
            hDesc.mShaderType = st;
            mParamHandleDescs.push_back(hDesc);
        }
    }
}

void GHMetalMaterial::applyCBBuffer(GHMaterialCallbackType::Enum ghIdx)
{
    const int numBufferTypesPerShaderType = (int)GHMTL_BI_PPERFRAME - (int)GHMTL_BI_VPERFRAME;
    
    for (int st = 0; st < ST_MAX; ++st)
    {
        if (!mShaderBindings[st].mConstantBuffers[ghIdx].mData)
        {
            continue;
        }
        GHMTL_BufferIndex metalBufferId = GHMetal::getMetalBufferIndex(ghIdx);
        metalBufferId = (GHMTL_BufferIndex)((int)metalBufferId + numBufferTypesPerShaderType * st);
        
        id<MTLRenderCommandEncoder> commandEncoder = mDevice.getRenderCommandEncoder();

        if (st == ST_VERTEX)
        {
            [commandEncoder setVertexBytes:mShaderBindings[st].mConstantBuffers[ghIdx].mData
                                     length:mShaderBindings[st].mConstantBuffers[ghIdx].mDataSize
                                    atIndex:(NSUInteger)metalBufferId];
        }
        else
        {
            [commandEncoder setFragmentBytes:mShaderBindings[st].mConstantBuffers[ghIdx].mData
                                     length:mShaderBindings[st].mConstantBuffers[ghIdx].mDataSize
                                    atIndex:(NSUInteger)metalBufferId];
        }

    }
}
