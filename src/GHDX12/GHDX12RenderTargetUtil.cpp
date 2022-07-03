#include "GHDX12RenderTargetUtil.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12/GHRenderDeviceDX12.h"
#include "GHDX12/GHDX12Helpers.h"

void GHDX12RenderTargetUtil::createDepthBuffers(GHRenderDeviceDX12& device, const GHRenderTarget::Config& config, const UINT numFrames, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, GHDX12RenderTargetUtil::FrameInfo* outFrames)
{
	// can probably share some of this code with GHRenderDeviceDX12.

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DEPTH_BUFFER_FORMAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, config.mWidth, config.mHeight);
	texDesc.Format = DEPTH_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DEPTH_BUFFER_FORMAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	if (config.mMsaa)
	{
		texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
		texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		texDesc.MipLevels = 1;
	}

	HRESULT hr;
	for (int frame = 0; frame < numFrames; ++frame)
	{
		hr = device.getDXDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&outFrames[frame].mDepthBuffer)
		);
		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to create rt depth buffer");
			return;
		}
		outFrames[frame].mDepthBuffer->SetName(L"RT Depth/Stencil Buffer");

		if (config.mMsaa && config.mType == GHRenderTarget::RTType::RT_DEPTHONLY)
		{
			D3D12_RESOURCE_DESC resolveDesc = texDesc;
			resolveDesc.SampleDesc.Count = 1;
			resolveDesc.SampleDesc.Quality = 0;
			resolveDesc.MipLevels = 0;

			hr = device.getDXDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resolveDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&depthOptimizedClearValue,
				IID_PPV_ARGS(&outFrames[frame].mResolveBuffer)
			);
			if (FAILED(hr))
			{
				GHDebugMessage::outputString("Failed to create rt depth resolve buffer");
				return;
			}
			outFrames[frame].mResolveBuffer->SetName(L"RT Depth/Stencil Resolve Buffer");
		}

		auto rtvDescriptorSize = device.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		outFrames[frame].mDepthBufferRTV = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		outFrames[frame].mDepthBufferRTV.ptr += rtvDescriptorSize * frame;

		device.getDXDevice()->CreateDepthStencilView(outFrames[frame].mDepthBuffer.Get(), &depthStencilDesc, outFrames[frame].mDepthBufferRTV);
	}
}

void GHDX12RenderTargetUtil::createColorBuffers(GHRenderDeviceDX12& device, const GHRenderTarget::Config& config, const UINT numFrames, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, FrameInfo* outFrames)
{
	// can probably share some of this code with GHRenderDeviceDX12.

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, config.mWidth, config.mHeight);
	texDesc.Format = SWAP_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (config.mMipmap && !config.mMsaa) texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (config.mMsaa)
	{
		texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
		texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
		texDesc.MipLevels = 1;
	}

	HRESULT hr;
	for (int frame = 0; frame < numFrames; ++frame)
	{
		hr = device.getDXDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			nullptr,
			IID_PPV_ARGS(&outFrames[frame].mColorBuffer)
		);
		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to create rt color buffer");
			return;
		}
		outFrames[frame].mColorBuffer->SetName(L"RT Color Buffer");

		if (config.mMsaa && config.mType == GHRenderTarget::RTType::RT_DEFAULT)
		{
			D3D12_RESOURCE_DESC resolveDesc = texDesc;
			resolveDesc.SampleDesc.Count = 1;
			resolveDesc.SampleDesc.Quality = 0;
			resolveDesc.MipLevels = 0;
			if (config.mMipmap) resolveDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			hr = device.getDXDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resolveDesc,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				nullptr,
				IID_PPV_ARGS(&outFrames[frame].mResolveBuffer)
			);
			if (FAILED(hr))
			{
				GHDebugMessage::outputString("Failed to create rt color resolve buffer");
				return;
			}
			outFrames[frame].mResolveBuffer->SetName(L"RT Color Resolve Buffer");
		}

		auto rtvDescriptorSize = device.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		outFrames[frame].mColorBufferRTV = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		outFrames[frame].mColorBufferRTV.ptr += rtvDescriptorSize * frame;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;
		if (config.mMsaa)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		}
		device.getDXDevice()->CreateRenderTargetView(outFrames[frame].mColorBuffer.Get(), &rtvDesc, outFrames[frame].mColorBufferRTV);
	}
}
