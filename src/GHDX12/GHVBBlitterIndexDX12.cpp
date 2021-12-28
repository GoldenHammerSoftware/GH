#include "GHVBBlitterIndexDX12.h"
#include "GHDX12Helpers.h"
#include "GHRenderDeviceDX12.h"
#include "GHVertexStreamDX12.h"
#include "Render/GHVertexBuffer.h"
#include "Render/GHShaderSemantic.h"
#include <assert.h>
#include "GHMath/GHHash.h"
#include "GHString/GHStringIdFactoryDJB2.h"

GHVBBlitterIndexDX12::GHVBBlitterIndexDX12(GHRenderDeviceDX12& device, unsigned int numIndices)
	: GHVBBlitterIndex(numIndices)
	, mDevice(device)
	, mBuffer(device, numIndices * sizeof(unsigned short), GHVBUsage::DYNAMIC)
{
	mDXView.BufferLocation = mBuffer.getDXBuffer()->GetGPUVirtualAddress();
	mDXView.Format = DXGI_FORMAT_R16_UINT; 
	mDXView.SizeInBytes = numIndices * sizeof(unsigned short);
}

GHVBBlitterIndexDX12::~GHVBBlitterIndexDX12(void)
{
	delete[] mD3DIED;
}

void GHVBBlitterIndexDX12::prepareVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->prepareVB((int)i, 0);
	}

	if (!mD3DIED) createIED(vb);

	mDevice.getRenderCommandList()->IASetIndexBuffer(&mDXView);
	mDevice.getRenderCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	if (mActiveIndices)
	{
		mDevice.getRenderCommandList()->DrawIndexedInstanced(mActiveIndices, 1, 0, 0, 0);
	}
}

unsigned short* GHVBBlitterIndexDX12::lockWriteBuffer(void)
{
	return (unsigned short*)mBuffer.lockWriteBuffer();
}

void GHVBBlitterIndexDX12::unlockWriteBuffer(void)
{
	mBuffer.unlockWriteBuffer();
}

const unsigned short* GHVBBlitterIndexDX12::lockReadBuffer(void) const
{
	return (unsigned short*)mBuffer.lockReadBuffer();
}

void GHVBBlitterIndexDX12::unlockReadBuffer(void) const
{
	mBuffer.unlockReadBuffer();
}

int GHVBBlitterIndexDX12::applyStreamComponentsToIED(const std::vector<GHVertexStreamFormat::ComponentEntry>& comps, int streamIdx, int localCompIdx)
{
	GHStringIdFactoryDJB2 stringIdFactory;
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

		GHHash::hash_combine(mIEDHash, mD3DIED[localCompIdx].Format);
		GHHash::hash_combine(mIEDHash, mD3DIED[localCompIdx].AlignedByteOffset);
		GHHash::hash_combine(mIEDHash, mD3DIED[localCompIdx].SemanticIndex);
		GHIdentifier nameId = stringIdFactory.generateHash(mD3DIED[localCompIdx].SemanticName);
		GHHash::hash_combine(mIEDHash, (GHIdentifier::IDType)nameId);

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
				GHDebugMessage::outputString("Unhandled component type in GHVBBlitterIndexDX12");
			};

			additionalComps.push_back(newComp);
		}
	}

	mIEDHash = 0;
	mD3DIED = new D3D12_INPUT_ELEMENT_DESC[mInputElementCount];
	int localCompIdx = 0;
	for (unsigned int streamIdx = 0; streamIdx < streams.size(); ++streamIdx)
	{
		const std::vector<GHVertexStreamFormat::ComponentEntry>& comps = streams[streamIdx]->get()->getFormat().getComponents();
		localCompIdx = applyStreamComponentsToIED(comps, streamIdx, localCompIdx);
	}
	localCompIdx = applyStreamComponentsToIED(additionalComps, 0, localCompIdx);
}
