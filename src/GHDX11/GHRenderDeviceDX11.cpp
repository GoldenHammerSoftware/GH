// Copyright Golden Hammer Software
#include "GHRenderDeviceDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProfiler.h"
#include "GHDX11SwapChainCreator.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHCamera.h"
#include "GHRenderStateMgrDX11.h"
#include "GHHMDRenderDevice.h"
#include "GHRenderTarget.h"

GHRenderDeviceDX11::GHRenderDeviceDX11(const int& graphicsQuality, GHMessageHandler& eventMgr)
: mClearColor(0, 0, 0, 0)
, mGraphicsQuality(graphicsQuality)
, mEventMgr(eventMgr)
, mStateMgr(0)
, mViewportClip(0, 0, 1, 1)
, mViewportSize(0,1)
{
}

GHRenderDeviceDX11::~GHRenderDeviceDX11(void)
{
}

bool GHRenderDeviceDX11::beginFrame(void)
{
	setDefaultRenderTarget();
	if (mStateMgr) mStateMgr->clearStates();

	if (mHMDDevice && mHMDDevice->isActive())
	{
		mHMDDevice->beginFrame();
	}
	return true;
}

void GHRenderDeviceDX11::endFrame(void) 
{
	/*
    // If the device was removed either by a disconnect or a driver upgrade, we 
    // must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        Initialize(mWindow->getCoreWindow().Get());
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
	*/

	if (mHMDDevice && mHMDDevice->isActive())
	{
		mVSyncEnabled = false;
		mHMDDevice->endFrame();
	}
	else
	{
		mVSyncEnabled = true;
	}
}

void GHRenderDeviceDX11::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
	mRenderTarget = optionalTarget;
	setClearColor(clearColor);
	if (mRenderTarget)
	{
		mRenderTarget->apply();
	}
	if (clearBuffers)
	{
		this->clearBuffers();
	}
}

void GHRenderDeviceDX11::endRenderPass(void)
{
	if (mRenderTarget)
	{
		mRenderTarget->remove();
		mRenderTarget = 0;
	}
}

void GHRenderDeviceDX11::setClearColor(const GHPoint4& color)
{
	mClearColor = color;
}

void GHRenderDeviceDX11::clearBuffers(void)
{
	clearZBuffer();
	clearBackBuffer();
}

void GHRenderDeviceDX11::clearZBuffer(void)
{
	if (!mActiveDepthStencilView) return;
	m_d3dContext->ClearDepthStencilView(
        mActiveDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH,
        1.0f,
        0
        );
}

void GHRenderDeviceDX11::clearBackBuffer(void) 
{
	if (!mActiveRenderTargetView) return;
	m_d3dContext->ClearRenderTargetView(
        mActiveRenderTargetView.Get(),
        mClearColor.getArr()
        );
}

void GHRenderDeviceDX11::applyViewInfo(const GHViewInfo& viewInfo)
{
	mViewInfo = viewInfo;
	setViewportClip(viewInfo.getViewportClip());
}

void GHRenderDeviceDX11::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
	GHViewInfo::ViewTransforms& deviceTransforms, const GHCamera& camera, bool /*isRenderToTexture*/) const
{
	deviceTransforms = engineTransforms;

	deviceTransforms.mPlatformGUITrans = GHTransform(
		2.0, 0.0, 0.0, 0.0,
		0.0, -2.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-1.0, 1.0, 0.0, 1.0
	);

	if (camera.getIsOrtho())
	{
		//D3DXMatrixOrthoRH 
		deviceTransforms.mProjectionTransform.becomeIdentity();
		deviceTransforms.mProjectionTransform.getMatrix()[0] = 2.0f / camera.getOrthoWidth();
		deviceTransforms.mProjectionTransform.getMatrix()[5] = 2.0f / camera.getOrthoHeight();
		deviceTransforms.mProjectionTransform.getMatrix()[10] = 1.0f / (camera.getNearClip() - camera.getFarClip());
		deviceTransforms.mProjectionTransform.getMatrix()[14] = camera.getNearClip() / (camera.getNearClip() - camera.getFarClip());

		deviceTransforms.mViewTransform.mult(deviceTransforms.mProjectionTransform,
			deviceTransforms.mViewProjTransform);
	}

	// d3d is our primary projection platform, so render to texture is the same as render to screen.
}

void GHRenderDeviceDX11::reinit(void)
{
	m_renderTargetView = nullptr;
	m_depthStencilView = nullptr;
	mActiveRenderTargetView = nullptr;
	mActiveDepthStencilView = nullptr;
	m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);

	createWindowSizeDependentResources();

	GHMessage reinitMessage(GHRenderProperties::DEVICEREINIT);
	mEventMgr.handleMessage(reinitMessage);
}

void GHRenderDeviceDX11::createDeviceResources(void)
{
}

// Allocate all memory resources that change on a window SizeChanged event.
void GHRenderDeviceDX11::createWindowSizeDependentResources()
{ 
	// we expect the following to be initialized before this function is called:
	//  m_renderTargetSize, m_renderTargetView, m_d3dDevice, m_d3dContext, mActiveRenderTargetView

	if (mMSAACount <= 1)
	{
		// no MSAA

		// Create a descriptor for the depth/stencil buffer.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			(UINT)m_renderTargetSize[0],
			(UINT)m_renderTargetSize[1],
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL);

		// Allocate a 2-D surface as the depth/stencil buffer.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
		HRESULT res;
		res = m_d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			depthStencil.GetAddressOf()
		);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to create depthStencil texture");

		// Create a DepthStencil view on this surface to use on bind.
		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		res = m_d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			m_depthStencilView.GetAddressOf()
		);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to create depthStencil view");
		mActiveDepthStencilView = m_depthStencilView;
	}
	else
	{
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			(UINT)m_renderTargetSize[0],
			(UINT)m_renderTargetSize[1],
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL,
			D3D11_USAGE_DEFAULT,
			0,
			mMSAACount, // must be the same as the backbuffer.
			0,
			0
		);


		// Allocate a 2-D surface as the depth/stencil buffer.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
		HRESULT res;
		res = m_d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			depthStencil.GetAddressOf()
		);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to create depthStencil texture");

		// Create a DepthStencil view on this surface to use on bind.
		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
		res = m_d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			m_depthStencilView.GetAddressOf()
		);
		if (FAILED(res)) GHDebugMessage::outputString("Failed to create depthStencil view");
		mActiveDepthStencilView = m_depthStencilView;
	}

	applyDefaultViewPort();
}

void GHRenderDeviceDX11::applyDefaultViewPort(void)
{
    // Create a viewport descriptor of the full window size.
	mViewportClip.setCoords(0, 0, 1, 1);
	mViewportSize.setCoords(m_renderTargetSize.getCoords());
	applyViewPort();
}

void GHRenderDeviceDX11::applyViewPort(void)
{
	CD3D11_VIEWPORT viewPort(
		mViewportClip[0] * mViewportSize[0],
		mViewportClip[1] * mViewportSize[1],
		mViewportClip[2] * mViewportSize[0] - mViewportClip[0] * mViewportSize[0],
		mViewportClip[3] * mViewportSize[1] - mViewportClip[1] * mViewportSize[1]
		);

	// Set the current viewport using the descriptor.
	m_d3dContext->RSSetViewports(1, &viewPort);
	assert(m_d3dContext);
}

void GHRenderDeviceDX11::setViewportSize(float width, float height)
{
	mViewportSize.setCoords(width, height);
	applyViewPort();
}

void GHRenderDeviceDX11::setViewportClip(const GHPoint4& newPort)
{
	mViewportClip = newPort;
	applyViewPort();
}

void GHRenderDeviceDX11::setDefaultRenderTarget(void)
{
	mActiveRenderTargetView = m_renderTargetView;
	mActiveDepthStencilView = m_depthStencilView;

	m_d3dContext->OMSetRenderTargets(
        1,
        mActiveRenderTargetView.GetAddressOf(),
        mActiveDepthStencilView.Get()
        );

	applyDefaultViewPort();
}

void GHRenderDeviceDX11::replaceRenderTarget(	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtView, 
												Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsView,
												Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& oldRtView,
												Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& oldDsView)
{
	oldRtView = mActiveRenderTargetView;
	oldDsView = mActiveDepthStencilView;

	mActiveRenderTargetView = rtView;
	mActiveDepthStencilView = dsView;

	m_d3dContext->OMSetRenderTargets(
        1,
        mActiveRenderTargetView.GetAddressOf(),
        mActiveDepthStencilView.Get()
        );
}

void GHRenderDeviceDX11::handleGraphicsQualityChange(void)
{
	reinit();
}
