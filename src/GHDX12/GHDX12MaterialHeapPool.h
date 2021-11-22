#pragma once

#include <vector>
#include "GHDX12Include.h"

class GHRenderDeviceDX12;

class GHDX12MaterialHeapPool
{
public:
	GHDX12MaterialHeapPool(GHRenderDeviceDX12& device);
	~GHDX12MaterialHeapPool(void);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> getDescriptorHeap(void);
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> getSamplerHeap(void);
	void getCBufferUploadHeap(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, UINT8*& gpuAddress, uint32_t bufferSize);

private:
	void checkNewFrame(void);

private:
	GHRenderDeviceDX12& mDevice;
	// separate set of pools for each frame.  expand to fit.
	std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mDescriptorHeapPools[NUM_SWAP_BUFFERS];
	std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mSamplerHeapPools[NUM_SWAP_BUFFERS];
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mCBufferUploadHeapPools[NUM_SWAP_BUFFERS];
	std::vector<UINT8*> mCBufferGPUAddresses[NUM_SWAP_BUFFERS];

	int32_t mCurrFrameIndex{ 0 };
	size_t mCurrDescriptorHeapIndex{ 0 };
	size_t mCurrSamplerHeapIndex{ 0 };
	size_t mCurrCBufferHeapIndex{ 0 };
};
