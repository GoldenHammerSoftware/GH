#pragma once

#include "GHXMLObjLoader.h"

class GHMetalRenderDevice;
class GHXMLObjLoaderGHM;
class GHMetalPipelineMgr;
class GHMetalShaderMgr;
class GHMaterialCallbackMgr;

class GHMetalMaterialXMLLoader : public GHXMLObjLoader
{
public:
    GHMetalMaterialXMLLoader(GHMetalRenderDevice& device,
                             const GHXMLObjLoaderGHM& ghmDescLoader,
                             GHMetalPipelineMgr& pipelineMgr,
                             GHMetalShaderMgr& shaderMgr,
                             const GHMaterialCallbackMgr& callbackMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const override;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const override;
    
private:
    GHMetalRenderDevice& mDevice;
    const GHXMLObjLoaderGHM& mGhmDescLoader;
    GHMetalPipelineMgr& mPipelineMgr;
    GHMetalShaderMgr& mShaderMgr;
    const GHMaterialCallbackMgr& mCallbackMgr;
};

