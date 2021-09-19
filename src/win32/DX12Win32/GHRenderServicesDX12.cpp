#include "GHRenderServicesDX12.h"

#include "NullPlatform/GHNullRenderDevice.h"
#include "NullPlatform/GHNullVBFactory.h"
#include "Render/GHFontRenderer.h"
#include "NullPlatform/GHNullRenderTargetFactory.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "Render/GHLightMgr.h"

GHRenderServicesDX12::GHRenderServicesDX12(GHSystemServices& systemServices)
: GHRenderServices(systemServices)
{
    GHRenderDevice* renderDevice = new GHNullRenderDevice();
    setDevice(renderDevice);
    setVBFactory(new GHNullVBFactory());
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHNullRenderTargetFactory();
    mLightMgr = new GHLightMgr(*mMaterialCallbackMgr);
}
