#include "GHRenderServicesDX12.h"

#include "GHRenderDeviceDX12.h"
#include "GHVBFactoryDX12.h"
#include "Render/GHFontRenderer.h"
#include "GHRenderTargetFactoryDX12.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "Render/GHXMLObjLoaderGHM.h"
#include "Base/GHAppShard.h"
#include "GHMaterialLoaderDX12.h"
#include "GHShaderLoaderDX12.h"
#include "Base/GHSystemServices.h"
#include "Render/GHShaderParamListLoader.h"
#include "Render/GHMaterialCallbackFactoryViewInfo.h"
#include "Render/GHMaterialCallbackMgr.h"
#include "GHWin32/GHWin32Window.h"
#include "GHTextureLoaderDX12.h"
#include "GHMipmapGeneratorDX12.h"
#include "GHUtils/GHRedirectResourceLoader.h"
#include "GHDX12MaterialHeapPool.h"

GHRenderServicesDX12::GHRenderServicesDX12(GHSystemServices& systemServices, GHWin32Window& window)
: GHRenderServices(systemServices)
, mWindow(window)
, mFileFinder(systemServices.getPlatformServices().getFileOpener())
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

    GHRedirectResourceLoader* shaderRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".cso");
    appShard.mResourceFactory.addLoader(shaderRedirect, 2, ".hlsl", ".glsl");

    GHDX12MaterialHeapPool* matHeapPool = new GHDX12MaterialHeapPool((GHRenderDeviceDX12&)*mRenderDevice);
    appShard.addOwnedItem(new GHTypedDeletionHandle<GHDX12MaterialHeapPool>(matHeapPool));

    GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmDesc for GHMaterialLoaderDX12");
    }
    else {
        GHMaterialLoaderDX12* ghmLoader = new GHMaterialLoaderDX12((GHRenderDeviceDX12&)*mRenderDevice, appShard.mResourceFactory,
            *mMaterialCallbackMgr,
            *ghmDescLoader,
            *matHeapPool);
        appShard.mXMLObjFactory.addLoader(ghmLoader, 1, "ghm");
    }

    GHMipmapGeneratorDX12* mipGen = new GHMipmapGeneratorDX12(appShard.mResourceFactory, (GHRenderDeviceDX12&)*mRenderDevice, *matHeapPool);
    appShard.addOwnedItem(new GHTypedDeletionHandle<GHMipmapGeneratorDX12>(mipGen));

    mRenderTargetFactory = new GHRenderTargetFactoryDX12((GHRenderDeviceDX12&)*mRenderDevice, *mipGen);

    GHTextureLoaderDX12* texLoader = new GHTextureLoaderDX12(mFileFinder, *((GHRenderDeviceDX12*)mRenderDevice), *mipGen);
    appShard.mResourceFactory.addLoader(texLoader, 4, ".jpg", ".png", ".pvr4", ".ovrtex");
    // todo: ghcm, dds
}
