// Copyright Golden Hammer Software
#include "GHRenderTargetDX11.h"
#include "GHTextureDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHPlatform/GHDebugMessage.h"

#define GHRENDERTARGET_MSAA_COUNT 4
#define GHRENDERTARGET_MSAA_QUALITY 0

// right now we only support one render target format.
// setting it as a define until we decide to support multiple formats.
#define GHRENDERTARGET_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define GHRENDERTARGET_DEPTH_FORMAT DXGI_FORMAT_R32_TYPELESS

GHRenderTargetDX11::GHRenderTargetDX11(GHRenderDeviceDX11& device, GHTextureDX11* tex, 
	const GHRenderTarget::Config& config, GHEventMgr& eventMgr)
: mDevice(device)
, mConfig(config)
, mRenderTargetView(nullptr)
, mDepthStencilTex(nullptr)
, mDepthStencilView(nullptr)
, mTexture(0)
, mEventMgr(eventMgr)
, mMessageListener(*this)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, tex);
	assert(mTexture);
	initTargets();
	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

GHRenderTargetDX11::~GHRenderTargetDX11(void)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, 0);
	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

void GHRenderTargetDX11::resize(const GHRenderTarget::Config& args)
{
	mConfig = args;
	initTargets();
}

void GHRenderTargetDX11::apply(void)
{
	mDevice.replaceRenderTarget(mRenderTargetView, mDepthStencilView, mReplacedRenderTargetView, mReplacedDepthStencilView);
	mReplacedViewportSize = mDevice.getViewportSize();
	mDevice.setViewportSize((float)mConfig.mWidth, (float)mConfig.mHeight);
}

void GHRenderTargetDX11::remove(void)
{
	mDevice.replaceRenderTarget(mReplacedRenderTargetView, mReplacedDepthStencilView, mReplacedRenderTargetView, mReplacedDepthStencilView);
	mDevice.setViewportSize(mReplacedViewportSize[0], mReplacedViewportSize[1]);

	// If we're MSAA then resolve to non-MSAA here.
	if (usesMSAA())
	{
		mDevice.getD3DContext()->ResolveSubresource(
			mResolveTex.Get(),
			0,
			mTargetTex.Get(),
			0,
			GHRENDERTARGET_FORMAT
		);
	}

	if (usesMips())
	{
		mDevice.getD3DContext()->GenerateMips(mTexture->getTextureView().Get());
	}
}

GHTexture* GHRenderTargetDX11::getTexture(void)
{
	return mTexture;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> GHRenderTargetDX11::initD3DTargetTex(bool isResolveTarget)
{
	// create a MSAA render target
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = mConfig.mWidth;
	textureDesc.Height = mConfig.mHeight;

	// Use 1 for a multisampled texture; or 0 to generate a full set of subtextures.
	if (usesMips()) textureDesc.MipLevels = 0;
	else textureDesc.MipLevels = 1;

	UINT sampleCount = 1;
	UINT sampleQuality = 0;
	if (!isResolveTarget && usesMSAA())
	{
		textureDesc.MipLevels = 1; // no mips on the MSAA target, the mips go on the resolve.
		sampleCount = GHRENDERTARGET_MSAA_COUNT;
		sampleQuality = GHRENDERTARGET_MSAA_QUALITY;
	}

	textureDesc.ArraySize = 1;
	textureDesc.Format = GHRENDERTARGET_FORMAT;
	textureDesc.SampleDesc.Count = sampleCount;
	textureDesc.SampleDesc.Quality = sampleQuality;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	if (usesMips()) textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	else textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dtex = nullptr;
	mDevice.getD3DDevice()->CreateTexture2D(&textureDesc, NULL, d3dtex.GetAddressOf());

	return d3dtex;
}

void GHRenderTargetDX11::initTargets(void)
{
	createDepthStencilView();
	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// don't create a color target if we are depth only
		// instead set up our texture to point to depth inside createDepthStencilView.
		mTargetTex = nullptr;
		mResolveTex = nullptr;
		return;
	}

	mTargetTex = initD3DTargetTex(false);
	// If we're msaa create a second target tex for the resolve.
	// This is the one we'll pass to future shaders.
	mResolveTex = mTargetTex;
	if (usesMSAA())
	{
		mResolveTex = initD3DTargetTex(true);
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = GHRENDERTARGET_FORMAT;
	if (usesMSAA())
	{
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	mRenderTargetView = nullptr;
	mDevice.getD3DDevice()->CreateRenderTargetView(mTargetTex.Get(), &renderTargetViewDesc, mRenderTargetView.GetAddressOf());

	// If msaa: create the shader resource view for the resolve target.
	// else: create the shader resource view for the render target.

	UINT srvMipLevels = 1;
	if (!usesMSAA() && usesMips())
	{
		// "Set to - 1 to indicate all the mipmap levels from MostDetailedMip on down to least detailed."
		srvMipLevels = -1;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = GHRENDERTARGET_FORMAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = srvMipLevels;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView = nullptr;
	mDevice.getD3DDevice()->CreateShaderResourceView(mResolveTex.Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());

	mTexture->setDeviceResources(mResolveTex, shaderResourceView);
}

void GHRenderTargetDX11::createDepthStencilView(void)
{
	// If we're a msaa target then we don't have mipmaps on our render view.
	// the mipmaps for msaa targets are generated after the resolve.

	// "Use 1 for a multisampled texture; or 0 to generate a full set of subtextures."
	UINT mipLevels = 1;
	if (usesMips()) mipLevels = 0;
	if (usesMSAA()) mipLevels = 1;

	UINT sampleCount = 1;
	UINT sampleQuality = 0;
	if (usesMSAA())
	{
		sampleCount = GHRENDERTARGET_MSAA_COUNT;
		sampleQuality = GHRENDERTARGET_MSAA_QUALITY;
	}
	
	UINT bindFlags = D3D11_BIND_DEPTH_STENCIL;
	UINT miscFlags = 0;
	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		bindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	}
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		GHRENDERTARGET_DEPTH_FORMAT,
		mConfig.mWidth,
		mConfig.mHeight,
		1, // array size
		mipLevels,
		bindFlags,
		D3D11_USAGE_DEFAULT,
		0, // cpuaccessFlags
		sampleCount,
		sampleQuality,
		miscFlags // miscFlags
	);

	mDepthStencilTex = nullptr;
	HRESULT res = mDevice.getD3DDevice()->CreateTexture2D(
		&depthStencilDesc,
		NULL,
		mDepthStencilTex.GetAddressOf()
	);
	if (res != S_OK)
	{
		GHDebugMessage::outputString("Failed to create depth target texture2d");
	}

	mDepthStencilView = nullptr;
	CD3D11_DEPTH_STENCIL_VIEW_DESC stencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
	if (usesMSAA()) stencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2DMS);
	stencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	mDevice.getD3DDevice()->CreateDepthStencilView(
		mDepthStencilTex.Get(),
		&stencilViewDesc,
		mDepthStencilView.GetAddressOf()
	);

	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// If we're depth only then make our external texture point to the depth buffer.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthRTView = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
		depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthSrvDesc.Texture2D.MipLevels = -1;
		depthSrvDesc.Texture2D.MostDetailedMip = 0;

		mDevice.getD3DDevice()->CreateShaderResourceView(mDepthStencilTex.Get(), &depthSrvDesc, depthRTView.GetAddressOf());
		mTexture->setDeviceResources(mDepthStencilTex, depthRTView);
	}
}

void GHRenderTargetDX11::reinit(void)
{
	mRenderTargetView = nullptr;
	mDepthStencilTex = nullptr;
	mDepthStencilView = nullptr;

	initTargets();
}

bool GHRenderTargetDX11::usesMSAA(void) const
{
	if (!mConfig.mMsaa)
	{
		return false;
	}
	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// todo: add depth-only msaa
		return false;
	}
	return true;
}

bool GHRenderTargetDX11::usesMips(void) const
{
	if (!mConfig.mMipmap)
	{
		return false;
	}
	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// can't make a target that can be depth and render target in order to generate mips.
		return false;
	}
	return true;
}