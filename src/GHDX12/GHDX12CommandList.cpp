#include "GHDX12CommandList.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Fence.h"

GHDX12CommandList::GHDX12CommandList(Microsoft::WRL::ComPtr<ID3D12Device2>& device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue)
	: mDXCommandQueue(commandQueue)
{
	HRESULT allocRes = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)mDXCommandAllocator.GetAddressOf());
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command allocator");
	}

	HRESULT listRes = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDXCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mDXCommandList.GetAddressOf()));
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command list");
	}
	mDXCommandList->Close();

	mFence = new GHDX12Fence(device);
}

GHDX12CommandList::~GHDX12CommandList(void)
{
	delete mFence;
}

void GHDX12CommandList::begin()
{
	mDXCommandAllocator->Reset();
	mDXCommandList->Reset(mDXCommandAllocator.Get(), nullptr);
}

void GHDX12CommandList::endAndSubmit()
{
	mDXCommandList->Close();
	ID3D12CommandList* ppCommandLists[1];
	ppCommandLists[0] = mDXCommandList.Get();
	mDXCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	mFenceWaitVal = mFence->addToCommandQueue(mDXCommandQueue);
}

void GHDX12CommandList::waitForCompletion()
{
	mFence->waitForFence(mFenceWaitVal);
}
