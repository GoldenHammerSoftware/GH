#include "GHDX12CBuffer.h"
#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"

GHDX12CBuffer::GHDX12CBuffer(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>* descriptorHeaps, size_t indexInHeap, size_t bufferSize)
	: mBufferSize(bufferSize)
    , mDescriptorHeaps(descriptorHeaps)
	, mDevice(device)
{
	mMemoryBuffer = (void*)(new char[bufferSize]);

    // CB size is required to be 256-byte aligned.
    size_t paddedBufferSize = (sizeof(mBufferSize) + 255) & ~255;

    D3D12_HEAP_PROPERTIES heapProperties;
    GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_UPLOAD);

    D3D12_RESOURCE_DESC bufferDesc;
    GHDX12Helpers::createBufferDesc(bufferDesc, (uint32_t)paddedBufferSize);

    for (int i = 0; i < NUM_SWAP_BUFFERS; ++i)
    {
        HRESULT hr = mDevice.getDXDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, 
            IID_PPV_ARGS(mUploadHeaps[i].GetAddressOf()));
        mUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

        const UINT cbvSrvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dHeap = mDescriptorHeaps[i];
        D3D12_CPU_DESCRIPTOR_HANDLE heapOffsetHandle = dHeap->GetCPUDescriptorHandleForHeapStart();
        heapOffsetHandle.ptr += (indexInHeap * cbvSrvDescriptorSize);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = mUploadHeaps[i]->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = paddedBufferSize;
        mDevice.getDXDevice()->CreateConstantBufferView(&cbvDesc, heapOffsetHandle);

        D3D12_RANGE readRange;
        readRange.Begin = 0;
        readRange.End = 0; // no read range, write only.
        hr = mUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&mGPUAddresses[i]));
    }
}

GHDX12CBuffer::~GHDX12CBuffer(void)
{
	delete[] mMemoryBuffer;
}

void GHDX12CBuffer::updateFrameData(size_t frameId)
{
    assert(frameId < NUM_SWAP_BUFFERS);

    memcpy(mGPUAddresses[frameId], mMemoryBuffer, mBufferSize);
}