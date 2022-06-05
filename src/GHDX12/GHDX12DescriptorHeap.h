#pragma once

#include "GHDX12Include.h"

class GHDX12DescriptorHeap
{
public:
	GHDX12DescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t size);

	D3D12_CPU_DESCRIPTOR_HANDLE getCPUDescriptorHandle(size_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandle(size_t index);
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> getDXDescriptorHeap(void) { return mDXDescriptorHeap; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDXDescriptorHeap{ nullptr };
	D3D12_DESCRIPTOR_HEAP_TYPE mType;
	size_t mSize;
	UINT mIncrement;
};