// Copyright Golden Hammer Software
#include "GHNullRenderServices.h"
#include "GHNullRenderDevice.h"
#include "GHNullVBFactory.h"
#include "Render/GHFontRenderer.h"
#include "GHNullRenderTargetFactory.h"
#include "GHPlatform/GHDeletionHandle.h"

GHNullRenderServices::GHNullRenderServices(GHSystemServices& systemServices)
: GHRenderServices(systemServices)
{
    GHRenderDevice* renderDevice = new GHNullRenderDevice();
    setDevice(renderDevice);
    setVBFactory(new GHNullVBFactory());
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHNullRenderTargetFactory();
}
