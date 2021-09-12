#include "GHMetalMaterialXMLLoader.h"
#include "GHMetalMaterial.h"
#include "GHXMLObjLoaderGHM.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMaterialCallbackMgr.h"

class GHMDesc;
class GHMetalShaderMgr;

GHMetalMaterialXMLLoader::GHMetalMaterialXMLLoader(GHMetalRenderDevice& device,
                                                   const GHXMLObjLoaderGHM& ghmDescLoader,
                                                   GHMetalPipelineMgr& pipelineMgr,
                                                   GHMetalShaderMgr& shaderMgr,
                                                   const GHMaterialCallbackMgr& callbackMgr)
: mDevice(device)
, mGhmDescLoader(ghmDescLoader)
, mPipelineMgr(pipelineMgr)
, mShaderMgr(shaderMgr)
, mCallbackMgr(callbackMgr)
{
}

void* GHMetalMaterialXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* desc = (GHMDesc*)mGhmDescLoader.create(node, extraData);
    if (!desc)
    {
        GHDebugMessage::outputString("Failed to load ghm for material");
        return 0;
    }
    
    // temp hack, add a param alias from DiffuseTexture to Texture0
    //desc->mParamAliases[GHString("DiffuseTexture", GHString::CHT_REFERENCE)] = GHString("Texture0", GHString::CHT_REFERENCE);
    
    GHMaterial* ret = new GHMetalMaterial(mDevice, mPipelineMgr, mShaderMgr, desc);
    mCallbackMgr.createCallbacks(*ret);
    return ret;
}

void GHMetalMaterialXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
}
