#ifdef GH_DX12

#include "GHOculusDX12RenderTarget.h"
#include "GHDX12/GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusUtil.h"
#include "GHDX12/GHDX12DescriptorHeap.h"

GHOculusDX12RenderTarget::GHOculusDX12RenderTarget(ovrSession session, GHRenderDeviceDX12& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize)
	: GHOculusRenderTarget(session, leftSize, rightSize)
	, mGHRenderDevice(ghRenderDevice) 
{
	mConfig.mWidth = mWidth;
	mConfig.mHeight = mHeight;
	mConfig.mMipmap = false;
	mConfig.mMsaa = false; // todo
	mConfig.mType = RT_DEFAULT;
	mConfig.mLabel.setConstChars("ovr target", GHString::CHT_REFERENCE);

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
		return;
	}

	int textureCount = 0;
	ovr_GetTextureSwapChainLength(session, mSwapChain, &textureCount);
	mFrames = new GHDX12RenderTargetUtil::FrameInfo[textureCount];

	assert(mColorDescriptorHeap == 0);
	mColorDescriptorHeap = new GHDX12DescriptorHeap(mGHRenderDevice.getDXDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, textureCount);
	mColorDescriptorHeap->getDXDescriptorHeap()->SetName(L"Oculus Color Resource Heap");

	for (int i = 0; i < textureCount; ++i)
	{
		result = ovr_GetTextureSwapChainBufferDX(session, mSwapChain, i, IID_PPV_ARGS(mFrames[i].mColorBuffer.GetAddressOf()));
		if (!OVR_SUCCESS(result))
		{
			GHDebugMessage::outputString("Failed to get dx12 swap chain buffer");
			continue;
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvd = {};
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		mFrames[i].mColorBufferRTV = mColorDescriptorHeap->getCPUDescriptorHandle(i);
		ghRenderDevice.getDXDevice().Get()->CreateRenderTargetView(mFrames[i].mColorBuffer.Get(), &rtvd, mFrames[i].mColorBufferRTV);
	}

	mDepthDescriptorHeap = new GHDX12DescriptorHeap(mGHRenderDevice.getDXDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, textureCount);
	mDepthDescriptorHeap->getDXDescriptorHeap()->SetName(L"Oculus Depth Resource Heap");

	GHDX12RenderTargetUtil::createDepthBuffers(mGHRenderDevice, mConfig, textureCount, mDepthDescriptorHeap->getDXDescriptorHeap(), mFrames);
}

GHOculusDX12RenderTarget::~GHOculusDX12RenderTarget(void)
{
	delete mFrames;
	delete mColorDescriptorHeap;
	delete mDepthDescriptorHeap;
}

void GHOculusDX12RenderTarget::apply(void)
{
	int currentIndex = 0;
	ovr_GetTextureSwapChainCurrentIndex(mSession, mSwapChain, &currentIndex);
	mGHRenderDevice.setIsStereo(true);
}

void GHOculusDX12RenderTarget::remove(void)
{
	mGHRenderDevice.setIsStereo(false);
}

GHTexture* GHOculusDX12RenderTarget::getTexture(void)
{
	return 0;
}

void GHOculusDX12RenderTarget::commitChanges(void)
{
}

#endif
