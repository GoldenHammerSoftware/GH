#include "GHDX12Fence.h"
#include "GHPlatform/GHDebugMessage.h"

GHDX12Fence::GHDX12Fence(Microsoft::WRL::ComPtr<ID3D12Device2> device)
{
	HRESULT fenceRes = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	if (fenceRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create command list fence");
	}
	mFenceVal = mFence->GetCompletedValue();
	mEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

GHDX12Fence::~GHDX12Fence(void)
{
	if (mFence) mFence->Release();
	if (mEvent) CloseHandle(mEvent);
}

uint64_t GHDX12Fence::addToCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue)
{
	mFenceVal++;
	commandQueue->Signal(mFence.Get(), mFenceVal);
	return mFenceVal;
}

bool GHDX12Fence::waitForFence(uint64_t value)
{
	uint64_t completedVal = mFence->GetCompletedValue();
	if (completedVal >= value)
	{
		return true;
	}
	HRESULT result = mFence->SetEventOnCompletion(value, mEvent);
	if (FAILED(result))
	{
		return false;
	}
	WaitForSingleObject(mEvent, INFINITE);
	return true;
}
