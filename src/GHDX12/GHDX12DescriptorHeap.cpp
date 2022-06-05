#include "GHDX12DescriptorHeap.h"
#include "GHDX12Helpers.h"

GHDX12DescriptorHeap::GHDX12DescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t size)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = size;
	heapDesc.Flags = flags;
	heapDesc.Type = type;
	HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mDXDescriptorHeap.GetAddressOf()));

	mIncrement = device->GetDescriptorHandleIncrementSize(type);
}

D3D12_CPU_DESCRIPTOR_HANDLE GHDX12DescriptorHeap::getCPUDescriptorHandle(size_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE heapCPUOffsetHandle = mDXDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	heapCPUOffsetHandle.ptr += (mIncrement * index);
	return heapCPUOffsetHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE GHDX12DescriptorHeap::getGPUDescriptorHandle(size_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE heapGPUOffsetHandle = mDXDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	heapGPUOffsetHandle.ptr += (mIncrement * index);
	return heapGPUOffsetHandle;
}
