#pragma once

#include "GHRenderTarget.h"
#include "OVR_CAPI_D3D.h"
#include <vector>
#include "GHDX11Include.h"
#include "GHMath/GHPoint.h"

class GHRenderDeviceDX11;

class GHOculusDX11RenderTarget : public GHRenderTarget
{
public:
	GHOculusDX11RenderTarget(ovrSession session, GHRenderDeviceDX11& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize);
	~GHOculusDX11RenderTarget(void);

	// make the target the place we render to.
	virtual void apply( void );
	// switch rendering back to how it was before apply.
	virtual void remove( void );

	virtual GHTexture* getTexture( void );

	void commitChanges(void);

	ovrTextureSwapChain getSwapChain(void) { return mSwapChain; }
	unsigned int getWidth(void) const { return mWidth; }
	unsigned int getHeight(void) const { return mHeight; }

	const ovrRecti& getViewport(int index) { return mEyeViewports[index]; }

private:
	GHRenderDeviceDX11&								mGHRenderDevice;
	ovrSession										mSession { nullptr };
	ovrTextureSwapChain								mSwapChain { nullptr };
	std::vector< Microsoft::WRL::ComPtr<ID3D11RenderTargetView> > mRenderTargetViews;
	unsigned int									mWidth;
	unsigned int									mHeight;

	unsigned int									mMSAACount{ 8 };
	ID3D11Texture2D*								mMSAATex;

	ovrRecti										mEyeViewports[ovrEye_Count];

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mDepthStencilTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	// keeping track in apply of what we've replaced so we can put it back in remove.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mReplacedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mReplacedDepthStencilView;
	GHPoint2 mReplacedViewportSize;
};