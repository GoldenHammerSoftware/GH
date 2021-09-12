// Copyright Golden Hammer Software
#include "GHRenderTargetFactoryDX11.h"
#include "GHRenderTargetDX11.h"
#include "GHTextureDX11.h"

GHRenderTargetFactoryDX11::GHRenderTargetFactoryDX11(GHRenderDeviceDX11& device, GHEventMgr& eventMgr)
: mDevice(device)
, mEventMgr(eventMgr)
{
}

GHRenderTarget* GHRenderTargetFactoryDX11::createRenderTarget(const GHRenderTarget::Config& args) const
{
	GHTextureDX11* tex = new GHTextureDX11(mDevice, nullptr, nullptr, &mEventMgr, "", 0,
											0, args.mWidth, args.mHeight, 4, true, DXGI_FORMAT_UNKNOWN);
	return new GHRenderTargetDX11(mDevice, tex, args, mEventMgr);
}

void GHRenderTargetFactoryDX11::resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const
{
	GHRenderTargetDX11* dx11Target = (GHRenderTargetDX11*)(&target);
	dx11Target->resize(args);
}