// Copyright 2010 Golden Hammer Software
#include "GHMetalRenderServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAppShard.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHSystemServices.h"

#include "GHMetalRenderDevice.h"
#include "GHMetalVBFactory.h"
#include "GHFontRenderer.h"
#include "GHMetalRenderTargetFactory.h"
#include "GHLightMgr.h"
#include "GHMetalMaterialXMLLoader.h"
#include "GHMetalPipelineMgr.h"
#include "GHMetalShaderMgr.h"
#include "GHMetalShaderLibraryResourceLoader.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHMaterialCallbackMgr.h"
#include "GHMetalTextureLoader.h"
#include "GHRedirectResourceLoader.h"
#include "GHResourceLoaderXML.h"
#include "GHMetalCubemapXMLLoader.h"

GHMetalRenderServices::GHMetalRenderServices(GHSystemServices& systemServices, GHiOSViewMetal& view)
: GHRenderServices(systemServices)
{
    GHMetalRenderDevice* renderDevice = new GHMetalRenderDevice(view, systemServices.getPlatformServices().getThreadFactory(), systemServices.getEventMgr(), getScreenInfo());
    setDevice(renderDevice);
    mPipelineMgr = new GHMetalPipelineMgr(*renderDevice);
    setVBFactory(new GHMetalVBFactory(*renderDevice));
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHMetalRenderTargetFactory(*renderDevice);
    mLightMgr = new GHLightMgr(*mMaterialCallbackMgr);
    
    mShaderLoader = new GHMetalShaderLibraryResourceLoader(systemServices.getPlatformServices().getFileOpener(), *renderDevice);
    mShaderMgr = new GHMetalShaderMgr(*renderDevice, *mShaderLoader);
    
    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderDevice->getViewInfo(), "");
    getMaterialCallbackMgr()->addFactory(viewCB);
}

GHMetalRenderServices::~GHMetalRenderServices(void)
{
    delete mPipelineMgr;
    delete mShaderMgr;
}

void GHMetalRenderServices::initAppShard(GHAppShard& appShard)
{
    GHRenderServices::initAppShard(appShard);
    
    GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmLoader for matLoader");
    }
    else {
        GHMetalMaterialXMLLoader* matLoader = new GHMetalMaterialXMLLoader(*(GHMetalRenderDevice*)mRenderDevice, *ghmDescLoader, *mPipelineMgr, *mShaderMgr, *mMaterialCallbackMgr);
        appShard.mXMLObjFactory.addLoader(matLoader, 1, "ghm");
    }
    
    appShard.mResourceFactory.addLoader(mShaderLoader, 1, ".metal");
    
    GHMetalTextureLoader* ktxLoader = new GHMetalTextureLoader(*(GHMetalRenderDevice*)mRenderDevice);
    appShard.mResourceFactory.addLoader(ktxLoader, 2, ".ktx", ".pvr");

    GHMetalTextureLoader* texLoader = new GHMetalTextureLoader(*(GHMetalRenderDevice*)mRenderDevice);
    appShard.mResourceFactory.addLoader(texLoader, 5, ".tga", ".jpg", ".bmp", ".png", ".PNG");
    
    GHRedirectResourceLoader* ktxRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".ktx");
    texLoader->addOverrideLoader(ktxRedirect);

    GHRedirectResourceLoader* pvrRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".pvr");
    texLoader->addOverrideLoader(pvrRedirect);

    GHResourceLoaderXML* cubemapResourceLoader = new GHResourceLoaderXML(appShard.mXMLObjFactory);
    appShard.mResourceFactory.addLoader(cubemapResourceLoader, 1, ".ghcm");

    GHMetalCubemapXMLLoader* cubemapXMLLoader = new GHMetalCubemapXMLLoader(appShard.mResourceFactory, *(GHMetalRenderDevice*)mRenderDevice);
    appShard.mXMLObjFactory.addLoader(cubemapXMLLoader, 1, "ghcm");
}
