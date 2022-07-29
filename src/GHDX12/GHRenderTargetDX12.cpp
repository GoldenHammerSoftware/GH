#include "GHRenderTargetDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"
#include "GHTextureDX12.h"
#include "GHMipmapGeneratorDX12.h"
#include "GHDX12DescriptorHeap.h"

GHRenderTargetDX12::GHRenderTargetDX12(GHRenderDeviceDX12& device, const GHRenderTarget::Config& args, GHMipmapGeneratorDX12& mipGen)
	: mConfig(args)
	, mDevice(device)
	, mMipGen(mipGen)
{
	// test hack disable 
	//mConfig.mMsaa = false;
	//mConfig.mMipmap = false;

	// disable mipmap if using depth texture.
	// todo: make GHMipmapGeneratorDX12 support this.
	if (mConfig.mType == GHRenderTarget::RTType::RT_DEPTHONLY)
	{
		mConfig.mMipmap = false;
	}

	createDXBuffers();
}

GHRenderTargetDX12::~GHRenderTargetDX12(void)
{
	for (int frame = 0; frame < NUM_SWAP_BUFFERS; ++frame)
	{
		if (mFrames[frame].mTexture) mFrames[frame].mTexture->release();
	}
	delete mColorDescriptorHeap;
	delete mDepthDescriptorHeap;
}

void GHRenderTargetDX12::apply(void)
{
	GHDX12RenderTargetUtil::applyRTGroup(mDevice, mConfig, mFrames[mDevice.getFrameBackendId()], SWAP_BUFFER_FORMAT, DEPTH_BUFFER_FORMAT, mViewport);
}

void GHRenderTargetDX12::remove(void)
{
	GHDX12RenderTargetUtil::removeRTGroup(mDevice, mFrames[mDevice.getFrameBackendId()]);

	if (mConfig.mMsaa)
	{
		resolveMsaa();
	}
	if (mConfig.mMipmap)
	{
		mMipGen.generateMipmaps(mFrames[mDevice.getFrameBackendId()].mTexture->getDXBuffer(), mFrames[mDevice.getFrameBackendId()].mTexture->getDXFormat(), mConfig.mWidth, mConfig.mHeight);
	}
}

GHTexture* GHRenderTargetDX12::getTexture(void)
{
	return mFrames[mDevice.getFrameBackendId()].mTexture;
}

void GHRenderTargetDX12::resize(const GHRenderTarget::Config& args)
{
	mConfig = args;
	mDevice.flushGPU();
	createDXBuffers();
}

void GHRenderTargetDX12::createDXBuffers(void)
{
	createDepthBuffers();
	createColorBuffers();

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = (FLOAT)mConfig.mWidth;
	mViewport.Height = (FLOAT)mConfig.mHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	for (int frame = 0; frame < NUM_SWAP_BUFFERS; ++frame)
	{
		if (mFrames[frame].mTexture) mFrames[frame].mTexture->release();

		DXGI_FORMAT outputFormat;
		Microsoft::WRL::ComPtr<ID3D12Resource> outputBuffer;

		if (mConfig.mType == GHRenderTarget::RTType::RT_DEFAULT)
		{
			outputFormat = SWAP_BUFFER_FORMAT;
			outputBuffer = mFrames[frame].mColorBuffer;
		}
		else
		{
			outputFormat = DEPTH_BUFFER_FORMAT_SRV;
			outputBuffer = mFrames[frame].mDepthBuffer;
		}
		if (mConfig.mMsaa)
		{
			outputBuffer = mFrames[frame].mResolveBuffer;
		}
		mFrames[frame].mTexture = new GHTextureDX12(mDevice, outputBuffer, outputFormat, mConfig.mMipmap);
		mFrames[frame].mTexture->acquire();
	}
}

void GHRenderTargetDX12::createDepthBuffers(void)
{
	mDepthDescriptorHeap = new GHDX12DescriptorHeap(mDevice.getDXDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, NUM_SWAP_BUFFERS);
	mDepthDescriptorHeap->getDXDescriptorHeap()->SetName(L"RT Depth Resource Heap");

	GHDX12RenderTargetUtil::createDepthBuffers(mDevice, mConfig, NUM_SWAP_BUFFERS, mDepthDescriptorHeap->getDXDescriptorHeap(), mFrames);
}

void GHRenderTargetDX12::createColorBuffers(void)
{
	// todo: don't create these for depth only targets.
	// might need a new root signature for that.
	mColorDescriptorHeap = new GHDX12DescriptorHeap(mDevice.getDXDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, NUM_SWAP_BUFFERS);
	mColorDescriptorHeap->getDXDescriptorHeap()->SetName(L"RT Color Resource Heap");

	GHDX12RenderTargetUtil::createColorBuffers(mDevice, mConfig, NUM_SWAP_BUFFERS, mColorDescriptorHeap->getDXDescriptorHeap(), mFrames);
}

void GHRenderTargetDX12::resolveMsaa(void)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> resolveSource = mFrames[mDevice.getFrameBackendId()].mColorBuffer;
	DXGI_FORMAT resolveFormat = SWAP_BUFFER_FORMAT;
	if (mConfig.mMsaa && mConfig.mType == GHRenderTarget::RTType::RT_DEPTHONLY)
	{
		resolveSource = mFrames[mDevice.getFrameBackendId()].mDepthBuffer;
		resolveFormat = DEPTH_BUFFER_FORMAT_SRV;
	}

	// transition resources to the resolve ready state.
	D3D12_RESOURCE_BARRIER srcInBarrier;
	srcInBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	srcInBarrier.Transition.pResource = resolveSource.Get();
	srcInBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	srcInBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	srcInBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	srcInBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &srcInBarrier);
	D3D12_RESOURCE_BARRIER dstInBarrier;
	dstInBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	dstInBarrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mResolveBuffer.Get();
	dstInBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	dstInBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	dstInBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	dstInBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &dstInBarrier);

	// do the resolve.
	mDevice.getRenderCommandList()->ResolveSubresource(mFrames[mDevice.getFrameBackendId()].mResolveBuffer.Get(), 0, resolveSource.Get(), 0, resolveFormat);

	// transition resources back.
	D3D12_RESOURCE_BARRIER srcOutBarrier;
	srcOutBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	srcOutBarrier.Transition.pResource = resolveSource.Get();
	srcOutBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	srcOutBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	srcOutBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	srcOutBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &srcOutBarrier);
	D3D12_RESOURCE_BARRIER dstOutBarrier;
	dstOutBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	dstOutBarrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mResolveBuffer.Get();
	dstOutBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	dstOutBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	dstOutBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	dstOutBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &dstOutBarrier);
}
