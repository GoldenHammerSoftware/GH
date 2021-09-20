#include "GHRenderServicesDX12.h"

#include "GHRenderDeviceDX12.h"
#include "NullPlatform/GHNullVBFactory.h"
#include "Render/GHFontRenderer.h"
#include "NullPlatform/GHNullRenderTargetFactory.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "Render/GHLightMgr.h"

GHRenderServicesDX12::GHRenderServicesDX12(GHSystemServices& systemServices, GHWin32Window& window)
: GHRenderServices(systemServices)
, mWindow(window)
{
    GHRenderDevice* renderDevice = new GHRenderDeviceDX12(mWindow);
    setDevice(renderDevice);
    setVBFactory(new GHNullVBFactory());
    mFontRenderer = new GHFontRenderer(*mVBFactory);
    addOwnedItem(new GHTypedDeletionHandle<GHFontRenderer>(mFontRenderer));
    mRenderTargetFactory = new GHNullRenderTargetFactory();
    mLightMgr = new GHLightMgr(*mMaterialCallbackMgr);
}
