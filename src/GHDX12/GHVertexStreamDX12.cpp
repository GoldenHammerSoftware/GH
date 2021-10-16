#include "GHVertexStreamDX12.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"

GHVertexStreamDX12::GHVertexStreamDX12(GHRenderDeviceDX12& device, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage)
	: GHVertexStream(format)
	, mDevice(device)
	, mUsage(usage)
{
	mNumVerts = numVerts;
	size_t bufferSize = mFormat->get()->getVertexSize() * mNumVerts * sizeof(float);
	mMemoryBuffer = new char[bufferSize];

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC bufferDesc;
	GHDX12Helpers::createBufferDesc(bufferDesc, format->get()->getVertexSize() * mNumVerts);

	device.getDXDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE, 
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mDXBuffer));
	mDXBuffer->SetName(L"Vertex stream buffer");

	D3D12_HEAP_PROPERTIES uploadHeapProperties;
	GHDX12Helpers::createHeapProperties(uploadHeapProperties, D3D12_HEAP_TYPE_UPLOAD);

	device.getDXDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mDXUploadBuffer));
	mDXUploadBuffer->SetName(L"Vertex stream upload buffer");

	mDXView.BufferLocation = mDXBuffer->GetGPUVirtualAddress();
	mDXView.StrideInBytes = mFormat->get()->getVertexSize();
	mDXView.SizeInBytes = bufferSize;
}

GHVertexStreamDX12::~GHVertexStreamDX12(void)
{
	delete[] mMemoryBuffer;
	// this is crashing, and not releasing them is probably a horrible leak.
	//mDXUploadBuffer->Release();
	//mDXBuffer->Release();
}

void GHVertexStreamDX12::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
}

void GHVertexStreamDX12::endVB(int streamIdx)
{
}

float* GHVertexStreamDX12::lockWriteBuffer(void)
{
	return (float*)mMemoryBuffer;
}

void GHVertexStreamDX12::unlockWriteBuffer(void)
{
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(mMemoryBuffer);
	size_t bufferSize = mFormat->get()->getVertexSize()* mNumVerts;
	vertexData.RowPitch = bufferSize;
	vertexData.SlicePitch = bufferSize;

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

const float* GHVertexStreamDX12::lockReadBuffer(void) const
{
	return (float*)mMemoryBuffer;
}

void GHVertexStreamDX12::unlockReadBuffer(void) const
{
}

GHVertexStream* GHVertexStreamDX12::clone(void) const
{
	GHVertexStreamDX12* ret = new GHVertexStreamDX12(mDevice, mFormat, mNumVerts, mUsage);

	float* retBuffer = ret->lockWriteBuffer();
	memcpy(retBuffer, mMemoryBuffer, mFormat->get()->getVertexSize() * mNumVerts * sizeof(float));
	ret->unlockWriteBuffer();
	return ret;
}
