#pragma once

#include "GHDX12Include.h"

class GHDX12Fence;

class GHDX12CommandList
{
public:
	GHDX12CommandList(Microsoft::WRL::ComPtr<ID3D12Device2>& device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);
	~GHDX12CommandList(void);

	void begin();
	void endAndSubmit();
	void waitForCompletion();

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> getDXCommandList(void) { return mDXCommandList; }

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mDXCommandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDXCommandAllocator{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mDXCommandList{ nullptr };
	GHDX12Fence* mFence{ nullptr };
	uint64_t mFenceWaitVal{ 0 };
};
