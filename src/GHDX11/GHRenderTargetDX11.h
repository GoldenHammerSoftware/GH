// Copyright Golden Hammer Software
#pragma once

#include "GHRenderTarget.h"
#include "GHDX11Include.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHMath/GHPoint.h"

class GHTextureDX11;
class GHRenderDeviceDX11;
class GHEventMgr;

class GHRenderTargetDX11 : public GHRenderTarget
{
public:
	GHRenderTargetDX11(GHRenderDeviceDX11& device, GHTextureDX11* tex, const GHRenderTarget::Config& args, GHEventMgr& eventMgr);
	~GHRenderTargetDX11(void);

	virtual void apply(void);
	virtual void remove(void);
	virtual GHTexture* getTexture(void);

	GHTextureDX11* getTextureDX11(void) { return mTexture; }
	const GHTextureDX11* getTextureDX11(void) const { return mTexture; }

	void resize(const GHRenderTarget::Config& args);

	void reinit(void);

private:
	void initTargets(void);
	// set isResolveTarget true if we're msaa and this is not the primary render target tex.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> initD3DTargetTex(bool isResolveTarget);
	void createDepthStencilView(void);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHRenderTargetDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHRenderTargetDX11& mParent;
	};

	bool usesMSAA(void) const;
	bool usesMips(void) const;

private:
	GHEventMgr& mEventMgr;
	MessageListener mMessageListener;
	GHRenderTarget::Config mConfig;

	GHTextureDX11* mTexture{ 0 };
	GHRenderDeviceDX11& mDevice;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTargetTex{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mResolveTex{ nullptr };

	// keeping track in apply of what we've replaced so we can put it back in remove.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mReplacedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mReplacedDepthStencilView;
	GHPoint2 mReplacedViewportSize;
};
