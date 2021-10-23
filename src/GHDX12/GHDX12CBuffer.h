#pragma once

#include "GHDX12Include.h"

class GHRenderDeviceDX12;

// wrapper for a single constant buffer
// we expect these to always be changed frame to frame.
class GHDX12CBuffer
{
public:
	GHDX12CBuffer(GHRenderDeviceDX12& device, size_t bufferSize);
	~GHDX12CBuffer(void);

	void* getMemoryBuffer(void) { return mMemoryBuffer; }
	void updateFrameData(size_t frameId);

private:
	GHRenderDeviceDX12& mDevice;

	// cpu only memory copy
	void* mMemoryBuffer{ nullptr };
	size_t mBufferSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeaps[NUM_SWAP_BUFFERS];
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeaps[NUM_SWAP_BUFFERS];
	UINT8* mGPUAddresses[NUM_SWAP_BUFFERS];
};
