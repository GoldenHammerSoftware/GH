// Copyright Golden Hammer Software
#pragma once

#include "GHRenderDeviceFactoryDX11.h"
#include "GHDX11Include.h"

class GHMessageHandler;

class GHRenderDeviceFactoryDX11Managed : public GHRenderDeviceFactoryDX11
{
public:
	GHRenderDeviceFactoryDX11Managed(ID3D11Device1* device, 
		ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView,
		GHMessageHandler& eventMgr);

	virtual GHRenderDeviceDX11* createDevice(const int& graphicsQuality, const GHScreenInfo& screenInfo);

private:
	ID3D11Device1* mDevice;
	ID3D11DeviceContext1* mContext;
	ID3D11RenderTargetView* mRenderTargetView;
	GHMessageHandler& mEventMgr;
};
