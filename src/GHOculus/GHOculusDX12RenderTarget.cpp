#ifdef GH_DX12

#include "GHOculusDX12RenderTarget.h"
#include "GHDX12/GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusUtil.h"

GHOculusDX12RenderTarget::GHOculusDX12RenderTarget(ovrSession session, GHRenderDeviceDX12& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize)
	: GHOculusRenderTarget(session, leftSize, rightSize)
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

	ovrResult result = ovr_CreateTextureSwapChainDX(mSession, ghRenderDevice.getCommandQueue().Get(), &desc, &mSwapChain);
	if (!GHOculusUtil::checkOvrResult(result, "ovr_CreateTextureSwapChainDX"))
	{
		GHDebugMessage::outputString("Failed to create oculus texture swap chain");
	}
	else
	{
		int textureCount = 0;
		ovr_GetTextureSwapChainLength(session, mSwapChain, &textureCount);
		mTexRtv.resize(textureCount);
		mTexResource.resize(textureCount);
		for (int i = 0; i < textureCount; ++i)
		{
			result = ovr_GetTextureSwapChainBufferDX(session, mSwapChain, i, IID_PPV_ARGS(&mTexResource[i]));
			if (!OVR_SUCCESS(result))
			{
				GHDebugMessage::outputString("Failed to get dx12 swap chain buffer");
				continue;
			}

			/* TODO: make a descriptor heap
			D3D12_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvd.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			mTexRtv[i] = DIRECTX.RtvHandleProvider.AllocCpuHandle(); // Gives new D3D12_CPU_DESCRIPTOR_HANDLE
			ghRenderDevice.getDXDevice().Get()->CreateRenderTargetView(mTexResource[i], &rtvd, mTexRtv[i]);
			*/
		}
	}

}

GHOculusDX12RenderTarget::~GHOculusDX12RenderTarget(void)
{
}

void GHOculusDX12RenderTarget::apply(void)
{
}

void GHOculusDX12RenderTarget::remove(void)
{
}

GHTexture* GHOculusDX12RenderTarget::getTexture(void)
{
	return 0;
}

void GHOculusDX12RenderTarget::commitChanges(void)
{
}

#endif
