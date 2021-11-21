#include "GHDX12MaterialDescriptorHeapPool.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHMaterialCallbackType.h"
#include "GHPlatform/GHDebugMessage.h"

GHDX12MaterialDescriptorHeapPool::GHDX12MaterialDescriptorHeapPool(GHRenderDeviceDX12& device)
	: mDevice(device)
{
}

GHDX12MaterialDescriptorHeapPool::~GHDX12MaterialDescriptorHeapPool(void)
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

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GHDX12MaterialDescriptorHeapPool::getDescriptorHeap(void)
{
	int32_t currFrameId = mDevice.getFrameBackendId();
	if (currFrameId != mCurrFrameIndex)
	{
		mCurrFrameIndex = currFrameId;
		mCurrHeapIndex = 0;
	}
	while (mHeapPools[mCurrFrameIndex].size() < mCurrHeapIndex + 1)
	{
		mHeapPools[mCurrFrameIndex].push_back(createNewHeap(mDevice));
	}
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret = mHeapPools[mCurrFrameIndex][mCurrHeapIndex];
	mCurrHeapIndex++;
	return ret;
}
