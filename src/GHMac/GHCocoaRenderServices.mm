// Copyright 2010 Golden Hammer Software
#include "GHCocoaRenderServices.h"
#include "GHCocoaOGLContext.h"
#include "GHRenderDeviceOGL.h"
#include "GHVBFactoryOGL.h"
#include "GHCocoaTextureLoader.h"
#include "GHOGLTextureMgr.h"
#include "GHMaterialOGLSHLoader.h"
#include "GHDeletionHandle.h"
#include "GHSystemServices.h"
#include "GHFontRenderer.h"
#include "GHRenderTargetFactoryOGL.h"
#include "GHAppShard.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHMaterialCallbackMgr.h"
#include "GHRenderPassMembershipXMLLoader.h"

GHCocoaRenderServices::GHCocoaRenderServices(GHSystemServices& systemServices, 
                                             GHMutex& renderMutex,
                                             GHCocoaView& cocoaView)
: GHRenderServices(systemServices)
, mRenderMutex(renderMutex)
{
    GHOGLContext* context = new GHCocoaOGLContext(&cocoaView, systemServices.mEventMgr, mScreenInfo);
    addOwnedItem(new GHTypedDeletionHandle<GHOGLContext>(context));
    GHRenderDeviceOGL* renderDevice = new GHRenderDeviceOGL(*context, renderMutex);
    setDevice(renderDevice);
    setVBFactory(new GHVBFactoryOGL());
    mFontRenderer = new GHFontRenderer(*mVBFactory, mScreenInfo);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    
    mTextureMgr = new GHOGLTextureMgr();
    addOwnedItem(new GHTypedDeletionHandle<GHOGLTextureMgr>(mTextureMgr));
    
    mRenderTargetFactory = new GHRenderTargetFactoryOGL(*mTextureMgr, &renderMutex);
    
    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderDevice->getViewInfo(), "");
    getMaterialCallbackMgr()->addFactory(viewCB);
    
    setGraphicsQuality(2);
}

void GHCocoaRenderServices::initAppShard(GHAppShard& appShard)
{
    GHRenderServices::initAppShard(appShard);

    GHCocoaTextureLoader* texLoader = new GHCocoaTextureLoader(*mTextureMgr, mRenderMutex);
    appShard.mResourceFactory.addLoader(texLoader, 4, ".tga", ".jpg", ".bmp", ".png");

    GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmLoader for OGLSHLoader");
    }
    else {
        GHMaterialOGLSHLoader* ghmLoader = new GHMaterialOGLSHLoader(mSystemServices.mPlatformServices->getFileOpener(),
                                                                     *mMaterialCallbackMgr,
                                                                     *ghmDescLoader);
        appShard.mXMLObjFactory.addLoader(ghmLoader, 1, "ghm");
    }
}

