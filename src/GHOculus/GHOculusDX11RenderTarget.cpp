#include "GHOculusDX11RenderTarget.h"
#include "GHRenderDeviceDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusUtil.h"

GHOculusDX11RenderTarget::GHOculusDX11RenderTarget(	ovrSession session, GHRenderDeviceDX11& ghRenderDevice, 
													const ovrSizei& leftSize, const ovrSizei& rightSize )
	: GHOculusRenderTarget( session, leftSize, rightSize )
	, mGHRenderDevice(ghRenderDevice)
{
	ovrTextureSwapChainDesc desc = {};

	desc.Type = ovrTexture_2D;
	desc.ArraySize = 1;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.SampleCount = 1; // ovr_CreateTextureSwapChainDX is not allowed to have MSAA.
	desc.MiscFlags = ovrTextureMisc_DX_Typeless;
	desc.BindFlags = ovrTextureBind_DX_RenderTarget;
	desc.StaticImage = ovrFalse;

	ovrResult result = ovr_CreateTextureSwapChainDX(mSession, ghRenderDevice.getD3DDevice().Get(), &desc, &mSwapChain);
	UINT msaaQuality = 0;
	if (GHOculusUtil::checkOvrResult(result, "ovr_CreateTextureSwapChainDX"))
	{
		D3D11_DSV_DIMENSION depthDSVDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		if (mMSAACount <= 1)
		{
			// no msaa, drawing directly to the oculus buffers.

			int textureCount = 0;
			ovr_GetTextureSwapChainLength(mSession, mSwapChain, &textureCount);
			mRenderTargetViews.reserve(textureCount);

			for (int i = 0; i < textureCount; ++i)
			{
				ID3D11Texture2D* tex = nullptr;
				ovr_GetTextureSwapChainBufferDX(mSession, mSwapChain, i, IID_PPV_ARGS(&tex));
				D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
				rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				ID3D11RenderTargetView* rtv;
				ghRenderDevice.getD3DDevice()->CreateRenderTargetView(tex, &rtvd, &rtv);
				mRenderTargetViews.push_back(rtv);
				tex->Release();
			}
		}
		else
		{
			// msaa: drawing to our own buffer that we then resolve to the oculus buffers.
			msaaQuality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
			//msaaQuality = D3D11_CENTER_MULTISAMPLE_PATTERN;
			//msaaQuality = 0;

			D3D11_TEXTURE2D_DESC dsDesc;
			dsDesc.Width = mWidth;
			dsDesc.Height = mHeight;
			dsDesc.MipLevels = 1; // "Use 1 for a multisampled texture"
			dsDesc.ArraySize = 1;
			dsDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dsDesc.SampleDesc.Count = mMSAACount;
			dsDesc.SampleDesc.Quality = msaaQuality;
			dsDesc.Usage = D3D11_USAGE_DEFAULT;
			dsDesc.CPUAccessFlags = 0;
			dsDesc.MiscFlags = 0;
			dsDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			dsDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

			ID3D11RenderTargetView* rtv;
			HRESULT texRes = ghRenderDevice.getD3DDevice()->CreateTexture2D(&dsDesc, NULL, &mMSAATex);
			HRESULT rtvRes = ghRenderDevice.getD3DDevice()->CreateRenderTargetView(mMSAATex, NULL, &rtv);
			mRenderTargetViews.push_back(rtv);
			if (texRes != S_OK || rtvRes != S_OK)
			{
				GHDebugMessage::outputString("Failed to create ovr render target");
			}

			depthDSVDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}

		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D32_FLOAT,//DXGI_FORMAT_D24_UNORM_S8_UINT,
			mWidth,
			mHeight,
			1, // array size
			1, // mip levels "Use 1 for a multisampled texture"
			D3D11_BIND_DEPTH_STENCIL,
			D3D11_USAGE_DEFAULT,
			0, // cpu access flags
			mMSAACount, // must be the same as the backbuffer.
			msaaQuality,
			0 // misc flags
		);

		mDepthStencilTex = nullptr;
		HRESULT depthStencilRes = mGHRenderDevice.getD3DDevice()->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			mDepthStencilTex.GetAddressOf()
		);
		if (depthStencilRes != S_OK)
		{
			GHDebugMessage::outputString("Failed to create depth stencil tex for ovr.");
		}
		
		mDepthStencilView = nullptr;
		HRESULT depthStencilViewRes = mGHRenderDevice.getD3DDevice()->CreateDepthStencilView(
			mDepthStencilTex.Get(),
			&CD3D11_DEPTH_STENCIL_VIEW_DESC(depthDSVDimension),
			mDepthStencilView.GetAddressOf()
		);
		if (depthStencilViewRes != S_OK)
		{
			GHDebugMessage::outputString("Failed to create depth stencil view for ovr");
		}

	}
	else
	{
		GHDebugMessage::outputString("Failed to create ovr swap chain");
	}
}

GHOculusDX11RenderTarget::~GHOculusDX11RenderTarget(void)
{
}

void GHOculusDX11RenderTarget::apply(void)
{
	int currentIndex = 0;
	ovr_GetTextureSwapChainCurrentIndex(mSession, mSwapChain, &currentIndex);

	if (mMSAACount <= 1)
	{
		mGHRenderDevice.replaceRenderTarget(mRenderTargetViews[currentIndex], mDepthStencilView, mReplacedRenderTargetView, mReplacedDepthStencilView);
	}
	else
	{
		// we only create 1 render target view for the msaa path.
		mGHRenderDevice.replaceRenderTarget(mRenderTargetViews[0], mDepthStencilView, mReplacedRenderTargetView, mReplacedDepthStencilView);
	}
	mReplacedViewportSize = mGHRenderDevice.getViewportSize();
	mGHRenderDevice.setViewportSize((float)mWidth, (float)mHeight);

	mGHRenderDevice.setIsStereo(true);
}

void GHOculusDX11RenderTarget::remove(void)
{
	mGHRenderDevice.replaceRenderTarget(mReplacedRenderTargetView, mReplacedDepthStencilView, mReplacedRenderTargetView, mReplacedDepthStencilView);
	mGHRenderDevice.setViewportSize(mReplacedViewportSize[0], mReplacedViewportSize[1]);

	mGHRenderDevice.setIsStereo(false);
}

void GHOculusDX11RenderTarget::commitChanges(void)
{
	if (mMSAACount > 1)
	{
		// resolve the msaa into the destination buffer.
		int destIndex = 0;
		ovr_GetTextureSwapChainCurrentIndex(mSession, mSwapChain, &destIndex);
		ID3D11Resource* dstTex = nullptr;
		ovr_GetTextureSwapChainBufferDX(mSession, mSwapChain, destIndex, IID_PPV_ARGS(&dstTex));
		mGHRenderDevice.getD3DContext()->ResolveSubresource(dstTex, 0, mMSAATex, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		dstTex->Release();
	}
	ovr_CommitTextureSwapChain(mSession, mSwapChain);
}

GHTexture* GHOculusDX11RenderTarget::getTexture(void)
{
	return nullptr;
}
