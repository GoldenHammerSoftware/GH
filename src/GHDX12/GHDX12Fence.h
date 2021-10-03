#pragma once

#include "GHDX12Include.h"

class GHDX12Fence
{
public:
	GHDX12Fence(Microsoft::WRL::ComPtr<ID3D12Device2> device);
	~GHDX12Fence(void);

	uint64_t addToCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);
	bool waitForFence(uint64_t value);

private:
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	HANDLE mEvent;
	uint64_t mFenceVal{ 0 };
};

