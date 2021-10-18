#include "GHDX12Buffer.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"

GHDX12Buffer::GHDX12Buffer(GHRenderDeviceDX12& device, size_t bufferSize, GHVBUsage::Enum usage)
	: mDevice(device)
	, mBufferSize(bufferSize)
	, mUsage(usage)
{
	mMemoryBuffer = (void*)(new char[bufferSize]);

	if (mUsage == GHVBUsage::DYNAMIC)
	{
		initDynamic();
	}
	else
	{
		initStatic();
	}
}

GHDX12Buffer::~GHDX12Buffer(void)
{
	delete[] mMemoryBuffer;
	// todo: delete dx buffers (crashing)
}

void* GHDX12Buffer::lockWriteBuffer(void)
{
	return mMemoryBuffer;
}

void GHDX12Buffer::unlockWriteBuffer(void)
{
	if (mUsage == GHVBUsage::STATIC)
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(mMemoryBuffer);
		vertexData.RowPitch = mBufferSize;
		vertexData.SlicePitch = mBufferSize;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = mDevice.beginUploadCommandList();
		GHDX12Helpers::UpdateSubresources(commandList.Get(), mDXBuffer, mDXUploadBuffer, 0, 0, 1, &vertexData);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mDXBuffer;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

		commandList->ResourceBarrier(1, &barrier);

		mDevice.endUploadCommandList();
	}
	else
	{
		BYTE* pData;
		HRESULT hr = mDXBuffer->Map(0, NULL, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			GHDebugMessage::outputString("Failed to map buffer for write");
			return;
		}
		memcpy(pData, mMemoryBuffer, mBufferSize);
		mDXBuffer->Unmap(0, NULL);
	}
}

const void* GHDX12Buffer::lockReadBuffer(void) const
{
	return mMemoryBuffer;
}

void GHDX12Buffer::unlockReadBuffer(void) const
{
}

void GHDX12Buffer::initStatic(void)
{
	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC bufferDesc;
	GHDX12Helpers::createBufferDesc(bufferDesc, (uint32_t)mBufferSize);

	mDevice.getDXDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mDXBuffer));
	mDXBuffer->SetName(L"gpu buffer");

	D3D12_HEAP_PROPERTIES uploadHeapProperties;
	GHDX12Helpers::createHeapProperties(uploadHeapProperties, D3D12_HEAP_TYPE_UPLOAD);

	mDevice.getDXDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mDXUploadBuffer));
	mDXUploadBuffer->SetName(L"upload buffer");
}

void GHDX12Buffer::initDynamic(void)
{
	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_UPLOAD);

	D3D12_RESOURCE_DESC bufferDesc;
	GHDX12Helpers::createBufferDesc(bufferDesc, (uint32_t)mBufferSize);

	HRESULT bufRes = mDevice.getDXDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mDXBuffer));
	if (FAILED(bufRes))
	{
		GHDebugMessage::outputString("Failed to create dx buffer");
	}
	mDXBuffer->SetName(L"upload buffer");
}
