#include "GHVBBlitterIndexDX12.h"
#include "GHDX12Helpers.h"
#include "GHRenderDeviceDX12.h"
#include "GHVertexStreamDX12.h"
#include "Render/GHVertexBuffer.h"
#include "Render/GHShaderSemantic.h"
#include <assert.h>

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
	delete[] mD3DIED;
	// todo: clean up dx buffers.
}

void GHVBBlitterIndexDX12::prepareVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->prepareVB((int)i, 0);
	}

	if (!mD3DIED) createIED(vb);
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

int GHVBBlitterIndexDX12::applyStreamComponentsToIED(const std::vector<GHVertexStreamFormat::ComponentEntry>& comps, int streamIdx, int localCompIdx)
{
	for (unsigned int compIdx = 0; compIdx < comps.size(); ++compIdx)
	{
		mD3DIED[localCompIdx].InputSlot = streamIdx;
		int semanticIdx = 0;
		mD3DIED[localCompIdx].SemanticName = GHShaderSemantic::getSemanticName(comps[compIdx].mComponent, semanticIdx);
		mD3DIED[localCompIdx].SemanticIndex = semanticIdx;
		mD3DIED[localCompIdx].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		mD3DIED[localCompIdx].InstanceDataStepRate = 0;
		mD3DIED[localCompIdx].AlignedByteOffset = comps[compIdx].mOffset * sizeof(float);

		if (comps[compIdx].mType == GHVertexComponentType::CT_BYTE)
		{
			if (comps[compIdx].mCount == 4) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			}
			else if (comps[compIdx].mCount == 2) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8G8_TYPELESS;
			}
			else if (comps[compIdx].mCount == 1) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8_TYPELESS;
			}
			else assert(false);
		}
		else if (comps[compIdx].mType == GHVertexComponentType::CT_UBYTE)
		{
			if (comps[compIdx].mCount == 4) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			else if (comps[compIdx].mCount == 2) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8G8_UNORM;
			}
			else if (comps[compIdx].mCount == 1) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R8_UNORM;
			}
			else assert(false);
		}
		else if (comps[compIdx].mType == GHVertexComponentType::CT_SHORT)
		{
			if (comps[compIdx].mCount == 2) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R16G16_UNORM;
			}
			else if (comps[compIdx].mCount == 1) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R16_UNORM;
			}
			else assert(false);
		}
		else if (comps[compIdx].mType == GHVertexComponentType::CT_FLOAT)
		{
			if (comps[compIdx].mCount == 4) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else if (comps[compIdx].mCount == 3) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (comps[compIdx].mCount == 2) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (comps[compIdx].mCount == 1) {
				mD3DIED[localCompIdx].Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else assert(false);
		}

		localCompIdx++;
	}
	return localCompIdx;
}

void GHVBBlitterIndexDX12::createIED(GHVertexBuffer& vb)
{
	bool foundComponents[GHVertexComponentShaderID::SI_MAX];
	for (int i = 0; i < (int)GHVertexComponentShaderID::SI_MAX; ++i)
	{
		foundComponents[i] = false;
	}

	assert(!mD3DIED);
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	mInputElementCount = 0;
	for (unsigned int streamIdx = 0; streamIdx < streams.size(); ++streamIdx)
	{
		const std::vector<GHVertexStreamFormat::ComponentEntry>& comps = streams[streamIdx]->get()->getFormat().getComponents();
		mInputElementCount += (unsigned int)comps.size();
		for (unsigned int compIdx = 0; compIdx < comps.size(); ++compIdx)
		{
			foundComponents[comps[compIdx].mComponent] = true;
		}
	}
	// Somewhat of a hack.  Avoid bad linkage errors by making sure our IED has dummy entries for all the default components.
	std::vector<GHVertexStreamFormat::ComponentEntry> additionalComps;
	for (int i = 0; i < (int)GHVertexComponentShaderID::SI_MAX; ++i)
	{
		if (!foundComponents[i])
		{
			mInputElementCount++;

			GHVertexStreamFormat::ComponentEntry newComp;
			newComp.mComponent = ((GHVertexComponentShaderID::Enum)i);
			newComp.mOffset = 0;

			// these are probably not all correct values.
			switch (newComp.mComponent)
			{
			case GHVertexComponentShaderID::SI_POS:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 3;
				break;
			case GHVertexComponentShaderID::SI_NORMAL:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 3;
				break;
			case GHVertexComponentShaderID::SI_TANGENT:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 3;
				break;
			case GHVertexComponentShaderID::SI_DIFFUSE:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 4;
				break;
			case GHVertexComponentShaderID::SI_SPECULAR:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 4;
				break;
			case GHVertexComponentShaderID::SI_BONEIDX:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 1;
				break;
			case GHVertexComponentShaderID::SI_BONEWEIGHT:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 3;
				break;
			case GHVertexComponentShaderID::SI_UV1:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 2;
				break;
			case GHVertexComponentShaderID::SI_UV2:
				newComp.mType = GHVertexComponentType::CT_FLOAT;
				newComp.mCount = 2;
				break;
			default:
				GHDebugMessage::outputString("Unhandled component type in GHVBBlitterIndexDX11");
			};

			additionalComps.push_back(newComp);
		}
	}

	mD3DIED = new D3D12_INPUT_ELEMENT_DESC[mInputElementCount];
	int localCompIdx = 0;
	for (unsigned int streamIdx = 0; streamIdx < streams.size(); ++streamIdx)
	{
		const std::vector<GHVertexStreamFormat::ComponentEntry>& comps = streams[streamIdx]->get()->getFormat().getComponents();
		localCompIdx = applyStreamComponentsToIED(comps, streamIdx, localCompIdx);
	}
	localCompIdx = applyStreamComponentsToIED(additionalComps, 0, localCompIdx);
}
