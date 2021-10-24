#include "GHRenderServicesDX12.h"

#include "GHRenderDeviceDX12.h"
#include "GHVBFactoryDX12.h"
#include "Render/GHFontRenderer.h"
#include "NullPlatform/GHNullRenderTargetFactory.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "Render/GHLightMgr.h"
#include "Render/GHXMLObjLoaderGHM.h"
#include "Base/GHAppShard.h"
#include "GHMaterialLoaderDX12.h"
#include "GHShaderLoaderDX12.h"
#include "Base/GHSystemServices.h"
#include "Render/GHShaderParamListLoader.h"
#include "Render/GHMaterialCallbackFactoryViewInfo.h"
#include "Render/GHMaterialCallbackMgr.h"
#include "GHWin32/GHWin32Window.h"

GHRenderServicesDX12::GHRenderServicesDX12(GHSystemServices& systemServices, GHWin32Window& window)
: GHRenderServices(systemServices)
, mWindow(window)
{
    const GHPoint2i& screenSize = window.getClientAreaSize();
    mScreenInfo.setSize(screenSize);
    mScreenInfo.setPixelSizeFactor(1 / 800.0f, 0.0f);
    mScreenInfo.setMinimumPixelHeight(600);

    GHRenderDeviceDX12 * renderDevice = new GHRenderDeviceDX12(mWindow);
    setDevice(renderDevice);
    setVBFactory(new GHVBFactoryDX12(*renderDevice));
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHNullRenderTargetFactory();
    mLightMgr = new GHLightMgr(*mMaterialCallbackMgr);

    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderDevice->getViewInfo(), "");
    mMaterialCallbackMgr->addFactory(viewCB);
}

void GHRenderServicesDX12::initAppShard(GHAppShard& appShard)
{
    GHRenderServices::initAppShard(appShard);

    GHShaderParamListLoader* splLoader = new GHShaderParamListLoader(mSystemServices.getXMLSerializer(), mSystemServices.getPlatformServices().getEnumStore());
    appShard.addOwnedItem(new GHTypedDeletionHandle<GHShaderParamListLoader>(splLoader));

    GHShaderLoaderDX12* shaderLoader = new GHShaderLoaderDX12(mSystemServices.getPlatformServices().getFileOpener(), *splLoader);
    appShard.mResourceFactory.addLoader(shaderLoader, 1, ".cso");

    GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmDesc for GHMaterialLoaderDX12");
    }
    else {
        GHMaterialLoaderDX12* ghmLoader = new GHMaterialLoaderDX12((GHRenderDeviceDX12&)*mRenderDevice, appShard.mResourceFactory,
            *mMaterialCallbackMgr,
            *ghmDescLoader);
        appShard.mXMLObjFactory.addLoader(ghmLoader, 1, "ghm");
    }
}
