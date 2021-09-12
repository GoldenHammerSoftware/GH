// Copyright Golden Hammer Software
#include "GHRenderServicesDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHVBFactoryDX11.h"
#include "GHFontRenderer.h"
#include "GHRenderTargetFactoryDX11.h"
#include "GHPlatform/GHDeletionHandle.h"
#include <cassert>
#include "GHSystemServices.h"
#include "GHAppShard.h"
#include "GHXMLObjLoaderGHM.h"
#include "GHMaterialLoaderDX11.h"
#include "GHMaterialCallbackMgr.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHRenderStateMgrDX11.h"
#include "GHRenderDeviceFactoryDX11.h"
#include "GHDX11SwapChainCreator.h"

#define ENUMHEADER "GHPropertiesDX11.h"
#include "GHString/GHEnumInclude.h"

GHRenderServicesDX11::GHRenderServicesDX11(GHSystemServices& systemServices,
										   GHRenderDeviceFactoryDX11* deviceFactory,
										   GHDX11SwapChainCreator* swapChainCreator,
										   const GHPoint2i& screenSize,
										   int initialGraphicsQuality)
: GHRenderServices(systemServices)
, mStateMgr(0)
, mDeviceFactory(deviceFactory)
, mSwapChainCreator(swapChainCreator)
{
	GHPropertiesDX11::generateIdentifiers(mSystemServices.getPlatformServices().getIdFactory());
	setGraphicsQuality(initialGraphicsQuality);

	mScreenInfo.setSize(screenSize);
	mScreenInfo.setPixelSizeFactor(1/800.0f, 0.0f);
	mScreenInfo.setMinimumPixelHeight(600);

    GHRenderDeviceDX11* renderDevice = mDeviceFactory->createDevice(mGraphicsQuality, mScreenInfo);
    setDevice(renderDevice);

	mStateMgr = new GHRenderStateMgrDX11(*renderDevice);
	renderDevice->setStateMgr(mStateMgr);

    setVBFactory(new GHVBFactoryDX11(*renderDevice, mActiveVS, systemServices.getEventMgr()));
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHRenderTargetFactoryDX11(*renderDevice, mSystemServices.getEventMgr());

	GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderDevice->getViewInfo(), "");
    mMaterialCallbackMgr->addFactory(viewCB);
}

GHRenderServicesDX11::~GHRenderServicesDX11(void)
{
	delete mStateMgr;
	delete mDeviceFactory;
	delete mSwapChainCreator;
}

void GHRenderServicesDX11::initAppShard(GHAppShard& appShard)
{
    GHRenderServices::initAppShard(appShard);    

	GHXMLObjLoaderGHM* ghmDescLoader = (GHXMLObjLoaderGHM*)appShard.mXMLObjFactory.getLoader("ghmDesc");
    if (!ghmDescLoader) {
        GHDebugMessage::outputString("Could not find ghmDesc for GHMaterialLoaderDX11");
    }
    else {
        GHMaterialLoaderDX11* ghmLoader = new GHMaterialLoaderDX11(appShard.mResourceFactory,
                                                                     *mMaterialCallbackMgr,
                                                                     *ghmDescLoader,
																	 *((GHRenderDeviceDX11*)mRenderDevice),
																	 *mStateMgr,
																	 mSystemServices.getEventMgr());
        appShard.mXMLObjFactory.addLoader(ghmLoader, 1, "ghm");
    }
}

GHRenderDeviceDX11* GHRenderServicesDX11::getDeviceDX11(void) const
{
	return (GHRenderDeviceDX11*)mRenderDevice;
}
