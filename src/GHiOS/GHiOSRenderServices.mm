// Copyright 2010 Golden Hammer Software
#ifndef GHMETAL

#include "GHiOSRenderServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHiOSGLESContext.h"
#include "GHiOSTextureLoader.h"
#include "GHiOSTextureLoaderPVRTC.h"
#include "GHAppShard.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHSystemServices.h"
#import <OPENGLES/ES2/glext.h>
#include "GHUtils/GHRedirectResourceLoader.h"

GHiOSRenderServices::GHiOSRenderServices(GHSystemServices& systemServices, 
                                         GHMutex& renderMutex,
                                         GHiOSViewOgles& view,
                                         bool allowMSAA,
                                         const GHStringIdFactory& hashTable)
: GHGLESRenderServices(systemServices, renderMutex, hashTable)
{
    int graphicsQuality = 2;
    setGraphicsQuality(graphicsQuality);
    
    EAGLContext* eaglContext = 0;
#if GH_ES_VERSION >= 3
    eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (eaglContext)
    {
        GHDebugMessage::outputString("Initializing ES3 Renderer");
    }
#endif
    if (!eaglContext)
    {
        eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (eaglContext)
        {
            GHDebugMessage::outputString("Initializing ES2 Renderer");
        }
    }
    if (!eaglContext || ![EAGLContext setCurrentContext:eaglContext])
    {
        GHDebugMessage::outputString("Failed to create an ES Context");
        return;
    }
    
    bool hasMSAA = false;
    if (graphicsQuality > 1 && allowMSAA) hasMSAA = true;
    GHiOSGLESContext* context = new GHiOSGLESContext([&view getEAGLLayer], eaglContext, hasMSAA, systemServices.getEventMgr());
    addOwnedItem(new GHTypedDeletionHandle<GHiOSGLESContext>(context));

    int screenWidth, screenHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &screenWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &screenHeight);
    mScreenInfo.setSize(GHPoint2i(screenWidth, screenHeight));
	mScreenInfo.setPixelSizeFactor(1/(float)(screenWidth), 0.0f);
    mScreenInfo.setMinimumPixelHeight(600);
    
    init(*context);
}

void GHiOSRenderServices::initAppShard(GHAppShard& appShard)
{
    GHGLESRenderServices::initAppShard(appShard);    
    
    GHiOSTextureLoader* texLoader = new GHiOSTextureLoader(*mTextureMgr, &mRenderMutex, appShard.mResourceFactory, mStateCache);
    appShard.mResourceFactory.addLoader(texLoader, 5, ".tga", ".jpg", ".bmp", ".png", ".PNG");
    
    GHiOSTextureLoaderPVRTC* pvrLoader = new GHiOSTextureLoaderPVRTC(*mTextureMgr, &mRenderMutex, mSystemServices.getPlatformServices().getFileOpener(), mStateCache);
    appShard.mResourceFactory.addLoader(pvrLoader, 2, ".pvr4", ".pvr");
    
    if (checkETC2Support())
    {
        GHRedirectResourceLoader* ktxRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".ktx");
        texLoader->addOverrideLoader(ktxRedirect);
    }
}

#endif
