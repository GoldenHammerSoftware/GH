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
	// todo: also transition depth.

	mDevice.applyRenderTarget(mFrames[mDevice.getFrameBackendId()].mColorBufferRTV, mFrames[mDevice.getFrameBackendId()].mDepthBufferRTV);
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
	// todo: also transition depth.

	mDevice.applyDefaultTarget();

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
		mFrames[frame].mTexture = new GHTextureDX12(mDevice, mFrames[frame].mColorBuffer, nullptr, SWAP_BUFFER_FORMAT, mConfig.mMipmap);
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
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, mConfig.mWidth, mConfig.mHeight);
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

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
	// same with format.

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
	// todo: support different formats.
	texDesc.Format = SWAP_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (mConfig.mMipmap) texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

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

		auto rtvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mFrames[frame].mColorBufferRTV = mColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		mFrames[frame].mColorBufferRTV.ptr += rtvDescriptorSize * frame;

		mDevice.getDXDevice()->CreateRenderTargetView(mFrames[frame].mColorBuffer.Get(), nullptr, mFrames[frame].mColorBufferRTV);
	}
}
