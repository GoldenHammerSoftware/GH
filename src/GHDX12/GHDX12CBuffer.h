#pragma once

#include "GHDX12Include.h"

class GHDX12MaterialHeapPool;
class GHRenderDeviceDX12;

// wrapper for a single constant buffer
// we expect these to always be changed frame to frame.
class GHDX12CBuffer
{
public:
	GHDX12CBuffer(GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& pool, size_t bufferSize);
	~GHDX12CBuffer(void);

	void* getMemoryBuffer(void) { return mMemoryBuffer; }
	size_t getSize(void) const { return mBufferSize; }
	
	void updateFrameData(void);
	void createSRV(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, size_t indexInHeap);

private:
	GHDX12MaterialHeapPool& mHeapPool;
	GHRenderDeviceDX12& mDevice;

	// cpu only memory copy
	void* mMemoryBuffer{ nullptr };
	size_t mBufferSize{ 0 };
	size_t mPaddedBufferSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeap;
	UINT8* mGPUAddress;
};
