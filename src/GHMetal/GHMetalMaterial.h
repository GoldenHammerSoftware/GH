#pragma once

#include "GHMaterial.h"
#include "GHMetalRenderDevice.h"
#include "GHMDesc.h"
#include "GHShaderType.h"

#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalPipelineMgr;
class GHMetalShaderMgr;

class GHMetalMaterial : public GHMaterial
{
public:
    GHMetalMaterial(GHMetalRenderDevice& device, GHMetalPipelineMgr& pipelineMgr,
                    GHMetalShaderMgr& shaderMgr, GHMDesc* desc);
    ~GHMetalMaterial(void);
    
    virtual void beginMaterial(const GHViewInfo& viewInfo) override;
    virtual void beginVB(const GHVertexBuffer& vb) override;
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) override;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) override;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) override;
    virtual void endMaterial(void) override;

    virtual GHMaterialParamHandle* getParamHandle(const char* paramName) override;
    
private:
    void createRenderPipelineState(void);
    void applyTextures(void);
    void initShaderReflection(void);
    void parseMTLArgument(MTLArgument* arg, GHShaderType st);
    void applyCBBuffer(GHMaterialCallbackType::Enum ghIdx);
    
private:
    GHMetalRenderDevice& mDevice;
    GHMetalPipelineMgr& mPipelineMgr;
    GHMetalShaderMgr& mShaderMgr;
    GHMDesc* mDesc{0};
    GHMDesc::ParamHandles* mDescParamHandles{ 0 };
    
    // if we used id<MTLBuffer> and setVertexBuffer we would need a separate copy for each instance.
    // using void* and setVertexBytes instead to avoid managing that.
    // this limits us to 4k buffer sizes so if we go over that we will need to re-evaluate.
    struct ShaderBuffer
    {
        void* mData{nullptr};
        size_t mDataSize{0};
    };
    struct ShaderBindings
    {
        ShaderBuffer mConstantBuffers[GHMaterialCallbackType::CT_MAX];
    };
    ShaderBindings mShaderBindings[ST_MAX];
    id<MTLFunction> mShaders[ST_MAX];
    
    id<MTLRenderPipelineState> mPipelineState;
    MTLDepthStencilDescriptor* mDepthDescriptor{0};
    id<MTLDepthStencilState> mDepthStencilState;
    
    struct ParamHandleDesc
    {
        GHString mName;
        size_t mOffset{0};
        GHMaterialCallbackType::Enum mCBType;
        bool mIsTexture{false};
        GHShaderType mShaderType{ST_VERTEX};
    };
    std::vector<ParamHandleDesc> mParamHandleDescs;
};

