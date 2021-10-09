// Copyright Golden Hammer Software
#include "GHRenderServices.h"
#include "GHRenderDevice.h"
#include "GHVBFactory.h"
#include "GHMaterialCallbackMgr.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHFontLoader.h"
#include "Base/GHSystemServices.h"
#include "GHMaterialReplacerXMLLoader.h"
#include "GHMaterialCallbackFactoryProperty.h"
#include "GHMaterialCallbackFactoryTransform.h"
#include "GHMaterialCallbackMgr.h"
#include "GHRenderTargetFactory.h"
#include "GHRenderPass.h"
#include "Base/GHAppShard.h"
#include "GHLightMgr.h"
#include "GHRenderPassMembershipXMLLoader.h"
#include "GHTextureLoaderHFF.h"
#include "GHXMLObjLoaderGHM.h"
#include "GHMaterialPassOverrideXMLLoader.h"
#include "GHParticleSystemDescXMLLoader.h"

#define ENUMHEADER "GHRenderProperties.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHMaterialCallbackType.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHVertexComponentShaderID.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHVertexComponentType.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHTextureFormat.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHTextureChannelType.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHHMDIdentifiers.h"
#include "GHString/GHEnumInclude.h"

GHRenderServices::GHRenderServices(GHSystemServices& systemServices)
: mVBFactory(0)
, mRenderDevice(0)
, mFontRenderer(0)
, mRenderTargetFactory(0)
, mMaterialCallbackMgr(new GHMaterialCallbackMgr)
, mSystemServices(systemServices)
, mGraphicsQuality(0)
{
    GHRenderProperties::generateIdentifiers(mSystemServices.getPlatformServices().getIdFactory());
    GHMaterialCallbackType::generateEnum(mSystemServices.getPlatformServices().getEnumStore());
    GHVertexComponentShaderID::generateEnum(mSystemServices.getPlatformServices().getEnumStore());
    GHVertexComponentType::generateEnum(mSystemServices.getPlatformServices().getEnumStore());
	GHTextureFormat::generateEnum(mSystemServices.getPlatformServices().getEnumStore());
	GHTextureChannelType::generateEnum(mSystemServices.getPlatformServices().getEnumStore());
	GHHMDIdentifiers::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());

    GHMaterialCallbackFactory* interpCallback = new GHMaterialCallbackFactoryProperty("InterpPct",
        GHMaterialCallbackType::CT_PERGEO, systemServices.getPlatformServices().getIdFactory().generateHash("interppct"), 
        GHMaterialParamHandle::HT_FLOAT);
    mMaterialCallbackMgr->addFactory(interpCallback);
        
    mLightMgr = new GHLightMgr(*mMaterialCallbackMgr);
    
    GHMaterialCallbackFactoryTransform* transCB = new GHMaterialCallbackFactoryTransform();
    mMaterialCallbackMgr->addFactory(transCB);
    
    GHRenderPass::sMainPassId = systemServices.getPlatformServices().getIdFactory().generateHash("main");
}

GHRenderServices::~GHRenderServices(void)
{
    delete mLightMgr;
    delete mMaterialCallbackMgr;
    delete mRenderTargetFactory;
    delete mVBFactory;
    delete mRenderDevice;
    
    for (size_t i = 0; i < mOwnedItems.size(); ++i)
    {
        delete mOwnedItems[i];
    }
}

void GHRenderServices::initAppShard(GHAppShard& appShard)
{
    appShard.mProps.setProperty(GHRenderProperties::GRAPHICSQUALITY, mGraphicsQuality);
    
    GHFontLoader* fontLoader = new GHFontLoader(appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(fontLoader, 1, "font");
    
    GHMaterialReplacerXMLLoader* matrepLoader = new GHMaterialReplacerXMLLoader(appShard.mXMLObjFactory,
                                                                                mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(matrepLoader, 1, "matreplace");
    
    GHRenderPassMembershipXMLLoader* passMemLoader = new GHRenderPassMembershipXMLLoader(mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(passMemLoader, 1, "renderPassMembership");
    
    GHMaterialPassOverrideXMLLoader* poLoader = new GHMaterialPassOverrideXMLLoader(appShard.mXMLObjFactory, mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(poLoader, 1, "materialPassOverride");
    
    GHXMLObjLoaderGHM* ghmLoader = new GHXMLObjLoaderGHM(appShard.mResourceFactory,
                                                         *passMemLoader,
                                                         *poLoader,
														 appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(ghmLoader, 1, "ghmDesc");
    
    GHTextureLoaderHFF* hffLoader = new GHTextureLoaderHFF(mSystemServices.getPlatformServices().getFileOpener());
    appShard.mResourceFactory.addLoader(hffLoader, 1, ".hff");

	GHParticleSystemDescXMLLoader* pdescLoader = new GHParticleSystemDescXMLLoader(appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(pdescLoader, 1, "particleDesc");
}

void GHRenderServices::setGraphicsQuality(int val) 
{ 
	if (mGraphicsQuality == val) {
		return;
	}
	mGraphicsQuality = val; 
	if (mRenderDevice) {
		mRenderDevice->handleGraphicsQualityChange();
	}
}
