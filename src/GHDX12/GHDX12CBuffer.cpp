#include "GHDX12CBuffer.h"
#include "GHDX12MaterialHeapPool.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"
#include "GHRenderDeviceDX12.h"

GHDX12CBuffer::GHDX12CBuffer(GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& pool, size_t bufferSize)
	: mBufferSize(bufferSize)
	, mHeapPool(pool)
    , mDevice(device)
{
	mMemoryBuffer = (void*)(new char[bufferSize]);

    // CB size is required to be 256-byte aligned.
    mPaddedBufferSize = (sizeof(mBufferSize) + 255) & ~255;
}

GHDX12CBuffer::~GHDX12CBuffer(void)
{
    mDevice.flushGPU();
	delete[] mMemoryBuffer;
}

void GHDX12CBuffer::updateFrameData(size_t frameId)
{
    assert(frameId < NUM_SWAP_BUFFERS);
    if (!mBufferSize) return;

    mHeapPool.getCBufferUploadHeap(mUploadHeap, mGPUAddress, mPaddedBufferSize);
    memcpy(mGPUAddress, mMemoryBuffer, mBufferSize);
}

void GHDX12CBuffer::createSRV(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, size_t indexInHeap, size_t frameId)
{
    const UINT cbvSrvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE heapOffsetHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    heapOffsetHandle.ptr += (indexInHeap * cbvSrvDescriptorSize);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mUploadHeap->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = mPaddedBufferSize;
    mDevice.getDXDevice()->CreateConstantBufferView(&cbvDesc, heapOffsetHandle);
}
