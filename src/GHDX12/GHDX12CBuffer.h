#pragma once

#include "GHDX12Include.h"

// wrapper for a single constant buffer
// we expect these to always be changed frame to frame.
class GHDX12CBuffer
{
public:
	GHDX12CBuffer(size_t bufferSize) {}
	~GHDX12CBuffer(void) {}

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap[NUM_SWAP_BUFFERS];
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeap[NUM_SWAP_BUFFERS];
	UINT8* mGPUAddresses[NUM_SWAP_BUFFERS];
};
