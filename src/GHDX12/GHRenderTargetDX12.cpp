#include "GHRenderTargetDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"
#include "GHTextureDX12.h"
#include "GHMipmapGeneratorDX12.h"

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
}

void GHRenderTargetDX12::apply(void)
{
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mColorBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER depthBarrier;
	depthBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	depthBarrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mDepthBuffer.Get();
	depthBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	depthBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	depthBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	depthBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &depthBarrier);

	GHDX12RTGroup rtGroup;
	rtGroup.mRt0 = mFrames[mDevice.getFrameBackendId()].mColorBufferRTV;
	rtGroup.mRt0Format = SWAP_BUFFER_FORMAT;
	rtGroup.mDepth = mFrames[mDevice.getFrameBackendId()].mDepthBufferRTV;
	rtGroup.mDepthFormat = DEPTH_BUFFER_FORMAT;
	if (mConfig.mMsaa)
	{
		rtGroup.mSampleCount = MSAA_SAMPLE_COUNT;
		rtGroup.mSampleQuality = MSAA_SAMPLE_QUALITY;
	}

	mDevice.applyRenderTarget(rtGroup);
	mDevice.getRenderCommandList()->RSSetViewports(1, &mViewport);
}

void GHRenderTargetDX12::remove(void)
{
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mColorBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER depthBarrier;
	depthBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	depthBarrier.Transition.pResource = mFrames[mDevice.getFrameBackendId()].mDepthBuffer.Get();
	depthBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	depthBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	depthBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	depthBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mDevice.getRenderCommandList()->ResourceBarrier(1, &depthBarrier);

	mDevice.applyDefaultTarget();

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
	// can probably share some of this code with GHRenderDeviceDX12.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = NUM_SWAP_BUFFERS;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = mDevice.getDXDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDepthDescriptorHeap));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create rt depth descriptor heap");
		return;
	}
	mDepthDescriptorHeap->SetName(L"RT Depth/Stencil Resource Heap");

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DEPTH_BUFFER_FORMAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, mConfig.mWidth, mConfig.mHeight);
	texDesc.Format = DEPTH_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DEPTH_BUFFER_FORMAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	if (mConfig.mMsaa)
	{
		texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
		texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		texDesc.MipLevels = 1;
	}

	for (int frame = 0; frame < NUM_SWAP_BUFFERS; ++frame)
	{
		hr = mDevice.getDXDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&mFrames[frame].mDepthBuffer)
		);
		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to create rt depth buffer");
			return;
		}
		mFrames[frame].mDepthBuffer->SetName(L"RT Depth/Stencil Buffer");

		if (mConfig.mMsaa && mConfig.mType == GHRenderTarget::RTType::RT_DEPTHONLY)
		{
			D3D12_RESOURCE_DESC resolveDesc = texDesc;
			resolveDesc.SampleDesc.Count = 1;
			resolveDesc.SampleDesc.Quality = 0;
			resolveDesc.MipLevels = 0;

			hr = mDevice.getDXDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resolveDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&depthOptimizedClearValue,
				IID_PPV_ARGS(&mFrames[frame].mResolveBuffer)
			);
			if (FAILED(hr))
			{
				GHDebugMessage::outputString("Failed to create rt depth resolve buffer");
				return;
			}
			mFrames[frame].mResolveBuffer->SetName(L"RT Depth/Stencil Resolve Buffer");
		}

		auto rtvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mFrames[frame].mDepthBufferRTV = mDepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		mFrames[frame].mDepthBufferRTV.ptr += rtvDescriptorSize * frame;

		mDevice.getDXDevice()->CreateDepthStencilView(mFrames[frame].mDepthBuffer.Get(), &depthStencilDesc, mFrames[frame].mDepthBufferRTV);
	}
}

void GHRenderTargetDX12::createColorBuffers(void)
{
	// todo: don't create these for depth only targets.
	// might need a new root signature for that.

	// can probably share some of this code with GHRenderDeviceDX12.
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = NUM_SWAP_BUFFERS;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = mDevice.getDXDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mColorDescriptorHeap));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create rt color descriptor heap");
		return;
	}
	mColorDescriptorHeap->SetName(L"RT Color Resource Heap");

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, mConfig.mWidth, mConfig.mHeight);
	texDesc.Format = SWAP_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (mConfig.mMipmap && !mConfig.mMsaa) texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (mConfig.mMsaa)
	{
		texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
		texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
		texDesc.MipLevels = 1;
	}

	for (int frame = 0; frame < NUM_SWAP_BUFFERS; ++frame)
	{
		hr = mDevice.getDXDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			nullptr,
			IID_PPV_ARGS(&mFrames[frame].mColorBuffer)
		);
		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to create rt color buffer");
			return;
		}
		mFrames[frame].mColorBuffer->SetName(L"RT Color Buffer");

		if (mConfig.mMsaa && mConfig.mType == GHRenderTarget::RTType::RT_DEFAULT)
		{
			D3D12_RESOURCE_DESC resolveDesc = texDesc;
			resolveDesc.SampleDesc.Count = 1;
			resolveDesc.SampleDesc.Quality = 0;
			resolveDesc.MipLevels = 0;
			if (mConfig.mMipmap) resolveDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			hr = mDevice.getDXDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resolveDesc,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				nullptr,
				IID_PPV_ARGS(&mFrames[frame].mResolveBuffer)
			);
			if (FAILED(hr))
			{
				GHDebugMessage::outputString("Failed to create rt color resolve buffer");
				return;
			}
			mFrames[frame].mResolveBuffer->SetName(L"RT Color Resolve Buffer");
		}

		auto rtvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mFrames[frame].mColorBufferRTV = mColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		mFrames[frame].mColorBufferRTV.ptr += rtvDescriptorSize * frame;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;
		if (mConfig.mMsaa)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		}
		mDevice.getDXDevice()->CreateRenderTargetView(mFrames[frame].mColorBuffer.Get(), &rtvDesc, mFrames[frame].mColorBufferRTV);
	}
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
