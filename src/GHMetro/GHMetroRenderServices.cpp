// Copyright 2010 Golden Hammer Software
#include "GHMetroRenderServices.h"
#include "GHTextureLoaderDX11.h"
#include "GHSystemServices.h"
#include "GHAppShard.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"
#include "GHMetroTextureFinder.h"
#include "GHTextureLoaderDDS.h"
#include "GHShaderLoaderDX11.h"

GHMetroRenderServices::GHMetroRenderServices(GHSystemServices& systemServices,
											 GHRenderDeviceFactoryDX11* deviceFactory,
											 GHDX11SwapChainCreator* swapChainCreator,
											 const GHPoint2i& screenSize)
: GHRenderServicesDX11(systemServices, deviceFactory, swapChainCreator, screenSize, 2)
, mTextureFinder(0)
{
	mTextureFinder = new GHMetroTextureFinder((GHMetroFileOpener&)(systemServices.getPlatformServices().getFileOpener()));
}

GHMetroRenderServices::~GHMetroRenderServices(void)
{
	delete mTextureFinder;
}

void GHMetroRenderServices::initAppShard(GHAppShard& appShard)
{
	GHRenderServicesDX11::initAppShard(appShard);
	
	GHShaderLoaderDX11* shaderLoader = new GHShaderLoaderDX11(mSystemServices.getPlatformServices().getFileOpener(),
		*((GHRenderDeviceDX11*)mRenderDevice), mActiveVS, mSystemServices.getXMLSerializer(),
		mSystemServices.getPlatformServices().getEnumStore(), mGraphicsQuality, "", mSystemServices.getEventMgr());
	appShard.mResourceFactory.addLoader(shaderLoader, 1, ".cso");

	GHTextureLoaderDDS* ddsLoader = new GHTextureLoaderDDS(*((GHRenderDeviceDX11*)mRenderDevice), 
		*mTextureFinder, mSystemServices.getEventMgr());
	appShard.mResourceFactory.addLoader(ddsLoader, 1, ".dds");

	GHTextureLoaderDX11* texLoader = new GHTextureLoaderDX11(*((GHRenderDeviceDX11*)mRenderDevice), 
		appShard.mResourceFactory, *mTextureFinder,
		mSystemServices.getEventMgr());
	appShard.mResourceFactory.addLoader(texLoader, 4, ".jpg", ".png", ".pvr4", ".bmp");
}
