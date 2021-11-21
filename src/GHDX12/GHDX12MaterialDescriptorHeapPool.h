#pragma once

#include <vector>
#include "GHDX12Include.h"

class GHRenderDeviceDX12;

class GHDX12MaterialDescriptorHeapPool
{
public:
	GHDX12MaterialDescriptorHeapPool(GHRenderDeviceDX12& device);
	~GHDX12MaterialDescriptorHeapPool(void);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> getDescriptorHeap(void);

private:
	GHRenderDeviceDX12& mDevice;
	// separate set of pools for each frame.  expand to fit.
	std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mHeapPools[NUM_SWAP_BUFFERS];

	size_t mCurrHeapIndex{ 0 };
	int32_t mCurrFrameIndex{ 0 };
};
