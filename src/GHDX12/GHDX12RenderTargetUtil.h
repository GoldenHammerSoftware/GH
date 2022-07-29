#pragma once

#include "GHDX12Include.h"
#include "Render/GHRenderTarget.h"

class GHTextureDX12;
class GHRenderDeviceDX12;

namespace GHDX12RenderTargetUtil
{
	struct FrameInfo
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mColorBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mColorBufferRTV;

		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mDepthBufferRTV;

		// optional msaa resolve.
		Microsoft::WRL::ComPtr<ID3D12Resource> mResolveBuffer;

		GHTextureDX12* mTexture{ 0 };
	};

	void createDepthBuffers(GHRenderDeviceDX12& device, const GHRenderTarget::Config& config, const UINT numFrames, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, FrameInfo* outFrames);
	void createColorBuffers(GHRenderDeviceDX12& device, const GHRenderTarget::Config& config, const UINT numFrames, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, FrameInfo* outFrames);

	void applyRTGroup(GHRenderDeviceDX12& device, const GHRenderTarget::Config& config, const FrameInfo& frameInfo, const DXGI_FORMAT colorFormat, const DXGI_FORMAT depthFormat, const D3D12_VIEWPORT& viewport);
	void removeRTGroup(GHRenderDeviceDX12& device, const FrameInfo& frameInfo);
};
