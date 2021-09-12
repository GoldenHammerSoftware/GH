// Copyright Golden Hammer Software
#pragma once

#include "GHRenderDeviceDX11.h"

class GHScreenInfo;

// A dx11 render device that does not manage the swapchain/backbuffer
class GHRenderDeviceDX11Managed : public GHRenderDeviceDX11
{
public:
	GHRenderDeviceDX11Managed(const int& graphicsQuality, const GHScreenInfo& screenInfo,
		ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView,
		GHMessageHandler& eventMgr);

	void updateSurfaces(ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView);
	virtual void createWindowSizeDependentResources(void);
	virtual GHTexture* resolveBackbuffer(void);

private:
	void updateRenderTargetSize(void);

private:
	// we need to keep track of which render target was given to us
	// because the parent class one is set to null on reinit.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mOSRenderTargetView;
	const GHScreenInfo& mScreenInfo;
};
