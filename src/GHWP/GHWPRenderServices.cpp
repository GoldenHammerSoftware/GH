// Copyright 2010 Golden Hammer Software
#include "GHWPRenderServices.h"
#include "GHAppShard.h"
#include "GHSystemServices.h"
#include "GHMetroFileOpener.h"
#include "GHTextureLoaderDDS.h"
#include "GHRedirectResourceLoader.h"
#include "GHShaderLoaderDX11.h"
#include "GHMetroTextureFinder.h"

GHWPRenderServices::GHWPRenderServices(GHSystemServices& systemServices,
									   GHRenderDeviceFactoryDX11* deviceFactory,
									   const GHPoint2i& screenSize)
: GHRenderServicesDX11(systemServices, deviceFactory, screenSize, 1)
{
	mTextureFinder = new GHMetroTextureFinder((GHMetroFileOpener&)(*systemServices.mFileOpener));
}

GHWPRenderServices::~GHWPRenderServices(void)
{
	delete mTextureFinder;
}

void GHWPRenderServices::initAppShard(GHAppShard& appShard)
{
	GHRenderServicesDX11::initAppShard(appShard);

	GHShaderLoaderDX11* shaderLoader = new GHShaderLoaderDX11(*mSystemServices.mFileOpener, 
		*((GHRenderDeviceDX11*)mRenderDevice), mActiveVS, mSystemServices.mXMLSerializer,
		mSystemServices.mEnumStore, mGraphicsQuality, "..\\..\\",
		mSystemServices.mEventMgr);
	appShard.mResourceFactory.addLoader(shaderLoader, 1, ".cso");

	GHTextureLoaderDDS* ddsLoader = new GHTextureLoaderDDS(*((GHRenderDeviceDX11*)mRenderDevice), 
		*mTextureFinder, mTextureBindArgs,
		mSystemServices.mEventMgr);
	appShard.mResourceFactory.addLoader(ddsLoader, 1, ".DDS");

	GHRedirectResourceLoader* pngLoader = new GHRedirectResourceLoader(appShard.mResourceFactory, ".dds");
	appShard.mResourceFactory.addLoader(pngLoader, 2, ".png", ".jpg");
}

