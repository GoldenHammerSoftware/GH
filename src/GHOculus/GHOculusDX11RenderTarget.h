#pragma once

#include "GHOculusRenderTarget.h"
#include "GHDX11Include.h"

class GHRenderDeviceDX11;

class GHOculusDX11RenderTarget : public GHOculusRenderTarget
{
public:
	GHOculusDX11RenderTarget(ovrSession session, GHRenderDeviceDX11& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize);
	~GHOculusDX11RenderTarget(void);

	// make the target the place we render to.
	virtual void apply( void );
	// switch rendering back to how it was before apply.
	virtual void remove( void );

	virtual GHTexture* getTexture( void );

	void commitChanges(void) override;

private:
	GHRenderDeviceDX11&								mGHRenderDevice;

	std::vector< Microsoft::WRL::ComPtr<ID3D11RenderTargetView> > mRenderTargetViews;
	unsigned int									mMSAACount{ 8 };
	ID3D11Texture2D*								mMSAATex;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mDepthStencilTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	// keeping track in apply of what we've replaced so we can put it back in remove.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mReplacedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mReplacedDepthStencilView;
	GHPoint2 mReplacedViewportSize;
};