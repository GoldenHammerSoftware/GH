// Copyright Golden Hammer Software
#include "GHWin32DX11RenderServices.h"
#include "GHSystemServices.h"
#include "GHAppShard.h"
#include "GHShaderLoaderDX11.h"
#include "GHWin32TextureFinder.h"
#include "GHUtils/GHRedirectResourceLoader.h"
#include "GHTextureLoaderDX11.h"
#include "GHTextureLoaderDDS.h"

GHWin32DX11RenderServices::GHWin32DX11RenderServices(GHSystemServices& systemServices,
													 GHRenderDeviceFactoryDX11* deviceFactory,
													 GHDX11SwapChainCreator* swapChainCreator,
													 const GHPoint2i& screenSize)
: GHRenderServicesDX11(systemServices, deviceFactory, swapChainCreator, screenSize, 2)
, mTextureFinder(new GHWin32TextureFinder(systemServices.getPlatformServices().getFileOpener()))
{
}

GHWin32DX11RenderServices::~GHWin32DX11RenderServices(void)
{
	delete mTextureFinder;
}

void GHWin32DX11RenderServices::initAppShard(GHAppShard& appShard)
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
	appShard.mResourceFactory.addLoader(texLoader, 4, ".jpg", ".png", ".pvr4", ".ovrtex");

	GHRedirectResourceLoader* cubemapLoader = new GHRedirectResourceLoader(appShard.mResourceFactory, ".dds");
	appShard.mResourceFactory.addLoader(cubemapLoader, 1, ".ghcm");
}
