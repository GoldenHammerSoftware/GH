#include "GHVBBlitterIndexDX12.h"
#include "GHDX12Helpers.h"
#include "GHRenderDeviceDX12.h"
#include <vector>
#include "GHVertexStreamDX12.h"
#include "Render/GHVertexBuffer.h"

GHVBBlitterIndexDX12::GHVBBlitterIndexDX12(GHRenderDeviceDX12& device, unsigned int numIndices)
	: GHVBBlitterIndex(numIndices)
	, mDevice(device)
{
	mMemoryBuffer = new char[numIndices * sizeof(unsigned short)];

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC bufferDesc;
	size_t ibSize = mNumIndices * sizeof(unsigned short);
	GHDX12Helpers::createBufferDesc(bufferDesc, ibSize);

	mDevice.getDXDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mDXBuffer));
	mDXBuffer->SetName(L"Index stream buffer");

	D3D12_HEAP_PROPERTIES uploadHeapProperties;
	GHDX12Helpers::createHeapProperties(uploadHeapProperties, D3D12_HEAP_TYPE_UPLOAD);

	device.getDXDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mDXUploadBuffer));
	mDXUploadBuffer->SetName(L"Index upload buffer");

	mDXView.BufferLocation = mDXBuffer->GetGPUVirtualAddress();
	mDXView.Format = DXGI_FORMAT_R16_UINT; 
	mDXView.SizeInBytes = ibSize;
}

GHVBBlitterIndexDX12::~GHVBBlitterIndexDX12(void)
{
	delete[] mMemoryBuffer;
	// todo: clean up dx buffers.
}

void GHVBBlitterIndexDX12::prepareVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->prepareVB((int)i, 0);
	}
}

void GHVBBlitterIndexDX12::endVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->endVB((int)i);
	}
}

void GHVBBlitterIndexDX12::blit(void)
{
}

unsigned short* GHVBBlitterIndexDX12::lockWriteBuffer(void)
{
	return (unsigned short*)mMemoryBuffer;
}

void GHVBBlitterIndexDX12::unlockWriteBuffer(void)
{
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(mMemoryBuffer);
	size_t bufferSize = mNumIndices * sizeof(unsigned short);
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

const unsigned short* GHVBBlitterIndexDX12::lockReadBuffer(void) const
{
	return (unsigned short*)mMemoryBuffer;
}

void GHVBBlitterIndexDX12::unlockReadBuffer(void) const
{
}
