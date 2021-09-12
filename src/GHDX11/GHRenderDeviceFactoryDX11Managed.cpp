// Copyright Golden Hammer Software
#include "GHRenderDeviceFactoryDX11Managed.h"
#include "GHRenderDeviceDX11Managed.h"

GHRenderDeviceFactoryDX11Managed::GHRenderDeviceFactoryDX11Managed(ID3D11Device1* device, 
	ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView,
	GHMessageHandler& eventMgr)
: mDevice(device)
, mContext(context)
, mRenderTargetView(renderTargetView)
, mEventMgr(eventMgr)
{
}

GHRenderDeviceDX11* GHRenderDeviceFactoryDX11Managed::createDevice(const int& graphicsQuality, const GHScreenInfo& screenInfo)
{
	return new GHRenderDeviceDX11Managed(graphicsQuality, screenInfo, mDevice, mContext, mRenderTargetView, mEventMgr);
}
