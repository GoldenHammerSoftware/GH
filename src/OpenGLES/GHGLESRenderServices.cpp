// Copyright 2010 Golden Hammer Software
#include "GHGLESRenderServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHGLESRenderDevice.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHMaterialCallbackMgr.h"
#include "GHFontRenderer.h"
#include "GHGLESTextureMgr.h"
#include "GHGLESRenderTargetFactory.h"
#include "GHAppShard.h"
#include "GHSystemServices.h"
#include "GHGLESMaterialSHLoader.h"
#include "GHGLESVBFactory.h"
#include "GHGLESShaderLoader.h"
#include "GHGLESReinitializer.h"
#include "GHGLESTextureFromDataFactoryLoader.h"
#include "GHTextureDataFactoryASTC.h"
#include "GHTextureDataFactoryKTX.h"
#include "GHGLESExtensions.h"
#include "GHGLESErrorReporter.h"
#include "GHGLESCubemapXMLLoader.h"
#include "GHResourceLoaderXML.h"

#define ENUMHEADER "GHGLESIdentifiers.h"
#include "GHString/GHEnumInclude.h"

GHGLESRenderServices::GHGLESRenderServices(GHSystemServices& systemServices, 
                                           GHMutex& renderMutex,
                                           const GHStringIdFactory& hashTable)
: GHRenderServices(systemServices)
, mRenderMutex(renderMutex)
, mStateCache(systemServices.getEventMgr())
{
    GHGLESIdentifiers::generateIdentifiers(hashTable);
    
    mTextureMgr = new GHGLESTextureMgr();
    addOwnedItem(new GHTypedDeletionHandle<GHGLESTextureMgr>(mTextureMgr));
    
    mReinitializer = new GHGLESReinitializer(*mTextureMgr, mShaderMgr, systemServices.getEventMgr());
    addOwnedItem(new GHTypedDeletionHandle<GHGLESReinitializer>(mReinitializer));
}

void GHGLESRenderServices::init(GHGLESContext& context)
{
    GHGLESRenderDevice* renderDevice = new GHGLESRenderDevice(context, mRenderMutex, mStateCache, mGraphicsQuality);
    setDevice(renderDevice);
    
    setVBFactory(new GHGLESVBFactory(mSystemServices.getEventMgr()));
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    
    mRenderTargetFactory = new GHGLESRenderTargetFactory(mSystemServices.getEventMgr(), *mTextureMgr, mStateCache, &mRenderMutex);
    
    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderDevice->getViewInfo(), "");
    getMaterialCallbackMgr()->addFactory(viewCB);
}

void GHGLESRenderServices::initAppShard(GHAppShard& appShard)
{
    GHRenderServices::initAppShard(appShard);    
    
    GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmLoader for OGLSHLoader");
    }
    else {
        GHGLESMaterialSHLoader* matLoader = new GHGLESMaterialSHLoader(appShard.mResourceFactory, 
            mSystemServices.getPlatformServices().getFileOpener(), *getMaterialCallbackMgr(), *ghmDescLoader, 
            mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getEventMgr(), 
            mShaderMgr, mStateCache, mGraphicsQuality);
        appShard.mXMLObjFactory.addLoader(matLoader, 1, "ghm");
    }
    
    GHGLESShaderLoader* shaderLoader = new GHGLESShaderLoader(mSystemServices.getPlatformServices().getFileOpener(), mSystemServices.getEventMgr());
    appShard.mResourceFactory.addLoader(shaderLoader, 1, ".glsl");

	// ktx isn't really a hardware format but rather a wrapper for hardware formats.
	// at the moment we are only using it for etc2, so check etc2 support.
	if (checkETC2Support())
	{
		GHTextureDataFactory* ktxDataFactory = new GHTextureDataFactoryKTX();
		GHGLESTextureFromDataFactoryLoader* ktxLoader = new GHGLESTextureFromDataFactoryLoader(*mTextureMgr, mStateCache, mSystemServices.getPlatformServices().getFileOpener(), ktxDataFactory);
		appShard.mResourceFactory.addLoader(ktxLoader, 1, ".ktx");
	}

	if (checkASTCSupport())
	{
		GHTextureDataFactory* astcDataFactory = new GHTextureDataFactoryASTC(); 
		GHGLESTextureFromDataFactoryLoader* astLoader = new GHGLESTextureFromDataFactoryLoader(*mTextureMgr, mStateCache, mSystemServices.getPlatformServices().getFileOpener(), astcDataFactory);
		appShard.mResourceFactory.addLoader(astLoader, 1, ".astc");
	}

	GHResourceLoaderXML* cubemapResourceLoader = new GHResourceLoaderXML(appShard.mXMLObjFactory);
	appShard.mResourceFactory.addLoader(cubemapResourceLoader, 1, ".ghcm");

	GHGLESCubemapXMLLoader* cubemapXMLLoader = new GHGLESCubemapXMLLoader(appShard.mResourceFactory, *mTextureMgr, mStateCache);
	appShard.mXMLObjFactory.addLoader(cubemapXMLLoader, 1, "ghcm");
}

bool GHGLESRenderServices::checkASTCSupport(void)
{
    if (GHGLESExtensions::checkExtension("GL_KHR_texture_compression_astc_hdr"))
    {
        return true;
    }
    if (GHGLESExtensions::checkExtension("GL_KHR_texture_compression_astc_ldr"))
    {
        return true;
    }
    return false;
}

bool GHGLESRenderServices::checkETC2Support(void)
{
    // Extensions don't show up in the list if they are rolled into the major version.
    // First check for version > 3 otherwise check the extensions list.

    if (GHGLESExtensions::checkAtLeastES3())
    {
        return true;
    }
    return GHGLESExtensions::checkExtension("compressed_ETC2_RGB");
}

GHGLESTextureMgr& GHGLESRenderServices::getTextureMgr(void)
{
	assert(mTextureMgr && "Error: GHGLESTextureMgr::mTextureMgr is null");
	return *mTextureMgr;
}

GHGLESStateCache& GHGLESRenderServices::getStateCache(void)
{
	return mStateCache;
}
