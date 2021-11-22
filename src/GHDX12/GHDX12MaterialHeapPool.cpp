#include "GHDX12MaterialHeapPool.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHMaterialCallbackType.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"

GHDX12MaterialHeapPool::GHDX12MaterialHeapPool(GHRenderDeviceDX12& device)
	: mDevice(device)
{
	for (int i = 0; i < NUM_SWAP_BUFFERS; ++i)
	{
		mDescriptorHeapPools[i].reserve(1024);
		mCBufferUploadHeapPools[i].reserve(1024);
		mCBufferGPUAddresses[i].reserve(1024);
	}
}

GHDX12MaterialHeapPool::~GHDX12MaterialHeapPool(void)
{
}

static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createNewHeap(GHRenderDeviceDX12& device)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret = 0;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 2 * GHMaterialCallbackType::CT_MAX + MAX_TEXTURES;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	HRESULT hr = device.getDXDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(ret.GetAddressOf()));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create cbuffer descriptor heap.");
	}
	return ret;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GHDX12MaterialHeapPool::getDescriptorHeap(void)
{
	checkNewFrame();
	while (mDescriptorHeapPools[mCurrFrameIndex].size() < mCurrDescriptorHeapIndex + 1)
	{
		mDescriptorHeapPools[mCurrFrameIndex].push_back(createNewHeap(mDevice));
	}
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret = mDescriptorHeapPools[mCurrFrameIndex][mCurrDescriptorHeapIndex];
	mCurrDescriptorHeapIndex++;
	return ret;
}

void GHDX12MaterialHeapPool::getCBufferUploadHeap(Microsoft::WRL::ComPtr<ID3D12Resource>& resource, UINT8*& gpuAddress, uint32_t bufferSize)
{
	static const uint32_t sBufferSize = 4096;
	assert(bufferSize <= sBufferSize);
	// There's potential for optimizing here by having multiple pools of different sizes.

	checkNewFrame();
	assert(mCBufferUploadHeapPools[mCurrFrameIndex].size() == mCBufferGPUAddresses[mCurrFrameIndex].size());
	while (mCBufferUploadHeapPools[mCurrFrameIndex].size() < mCurrCBufferHeapIndex + 1)
	{
		D3D12_HEAP_PROPERTIES heapProperties;
		GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_UPLOAD);

		D3D12_RESOURCE_DESC bufferDesc;
		GHDX12Helpers::createBufferDesc(bufferDesc, sBufferSize);

		Microsoft::WRL::ComPtr<ID3D12Resource> newHeap;
		HRESULT hr = mDevice.getDXDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(newHeap.GetAddressOf()));
		newHeap->SetName(L"Constant Buffer Upload Resource Heap");

		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to create cbuffer upload heap");
		}

		UINT8* newHeapAddr;
		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0; // no read range, write only.
		hr = newHeap->Map(0, &readRange, reinterpret_cast<void**>(&newHeapAddr));

		mCBufferUploadHeapPools[mCurrFrameIndex].push_back(newHeap);
		mCBufferGPUAddresses[mCurrFrameIndex].push_back(newHeapAddr);
	}
	resource = mCBufferUploadHeapPools[mCurrFrameIndex][mCurrCBufferHeapIndex];
	gpuAddress = mCBufferGPUAddresses[mCurrFrameIndex][mCurrCBufferHeapIndex];
	mCurrCBufferHeapIndex++;
}

void GHDX12MaterialHeapPool::checkNewFrame(void)
{
	int32_t currFrameId = mDevice.getFrameBackendId();
	if (currFrameId != mCurrFrameIndex)
	{
		mCurrFrameIndex = currFrameId;
		mCurrDescriptorHeapIndex = 0;
		mCurrCBufferHeapIndex = 0;
	}
}

