// Copyright Golden Hammer Software
#pragma once

#include "Render/GHRenderDevice.h"
#include "Render/GHViewInfo.h"
#include "GHDX11Include.h"

class GHMessageHandler;
class GHRenderStateMgrDX11;

class GHRenderDeviceDX11 : public GHRenderDevice
{
public:
	GHRenderDeviceDX11(const int& graphicsQuality, GHMessageHandler& eventMgr);
	~GHRenderDeviceDX11(void);

	void reinit(void);

    virtual bool beginFrame(void);
	virtual void endFrame(void);
    
	virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers);
	virtual void endRenderPass(void);
	
    virtual void applyViewInfo(const GHViewInfo& viewInfo);
	virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
		GHViewInfo::ViewTransforms& deviceTransforms,
		const GHCamera& camera, bool isRenderToTexture) const;
    virtual const GHViewInfo& getViewInfo(void) const { return mViewInfo; }

	virtual void handleGraphicsQualityChange(void);
	const int& getGraphicsQuality(void) const { return mGraphicsQuality; }

	// set the render target back to the one that is displayed on blit
	void setDefaultRenderTarget(void);
	// replace the render target for subsequent calls to clearBuffers etc.
	void replaceRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtView, 
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsView,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& oldRtView,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& oldDsView);

	Microsoft::WRL::ComPtr<ID3D11Device1> getD3DDevice(void) { return m_d3dDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> getD3DContext(void) { return m_d3dContext; }

	void setStateMgr(GHRenderStateMgrDX11* stateMgr) { mStateMgr = stateMgr; }

	void setViewportSize(float width, float height);
	const GHPoint2& getViewportSize(void) const { return mViewportSize; }
	void setViewportClip(const GHPoint4& newPort);

protected:
	virtual void createDeviceResources();
    virtual void createWindowSizeDependentResources();

	void setClearColor(const GHPoint4& color);
	void clearBuffers(void);
	void clearZBuffer(void);
	void clearBackBuffer(void);

private:
	void applyDefaultViewPort(void);
	void applyViewPort(void);

protected:
    GHViewInfo mViewInfo;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
	D3D_FEATURE_LEVEL                               m_featureLevel;
    GHPoint2										m_renderTargetSize;
	unsigned int									mMSAACount{ 1 }; // 1 means no MSAA
	// render passes might want to switch the render targer,
	// we need to keep track of which are active in order for the clear buffers calls to work.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  mActiveRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  mActiveDepthStencilView;
	const int& mGraphicsQuality;
	GHMessageHandler& mEventMgr;
	GHRenderStateMgrDX11* mStateMgr;
	bool mVSyncEnabled{ true };
	
private:
	GHPoint4 mClearColor;
	// pct of the default viewport that we use
	GHPoint4 mViewportClip;
	GHPoint2 mViewportSize;
	GHRenderTarget* mRenderTarget{ 0 };
};
