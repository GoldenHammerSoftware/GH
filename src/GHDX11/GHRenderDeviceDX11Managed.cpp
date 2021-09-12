// Copyright Golden Hammer Software
#include "GHRenderDeviceDX11Managed.h"
#include "GHScreenInfo.h"

GHRenderDeviceDX11Managed::GHRenderDeviceDX11Managed(const int& graphicsQuality, const GHScreenInfo& screenInfo,
	ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView,
	GHMessageHandler& eventMgr)
: GHRenderDeviceDX11(graphicsQuality, eventMgr)
, mScreenInfo(screenInfo)
{
	m_d3dDevice = device;
	m_d3dContext = context;
	m_renderTargetView = renderTargetView;
	mOSRenderTargetView = renderTargetView;

	updateRenderTargetSize();
	createDeviceResources();
	createWindowSizeDependentResources();
}

void GHRenderDeviceDX11Managed::updateSurfaces(ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView)
{
	if (renderTargetView != m_renderTargetView.Get())
	{
		m_renderTargetView = renderTargetView;
		mOSRenderTargetView = renderTargetView;
		mActiveRenderTargetView = m_renderTargetView;
	}
	if (device != m_d3dDevice.Get() || context != m_d3dContext.Get())
	{
		m_d3dDevice = device;
		m_d3dContext = context;
		updateRenderTargetSize();

		reinit();
	}
}

void GHRenderDeviceDX11Managed::createWindowSizeDependentResources(void)
{
	m_renderTargetView = mOSRenderTargetView;
	updateRenderTargetSize();
	GHRenderDeviceDX11::createWindowSizeDependentResources();
}

void GHRenderDeviceDX11Managed::updateRenderTargetSize(void)
{
	// need to figure out how big the backbuffer is now
	// this may or may not be accurate based on if dpi is used
	m_renderTargetSize[0] = (float)mScreenInfo.getSize()[0];
	m_renderTargetSize[1] = (float)mScreenInfo.getSize()[1];
}

GHTexture* GHRenderDeviceDX11Managed::resolveBackbuffer(void)
{
	//todo: implement
	return nullptr;
}
