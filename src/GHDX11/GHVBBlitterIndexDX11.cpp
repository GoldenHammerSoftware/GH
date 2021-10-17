// Copyright Golden Hammer Software
#include "GHVBBlitterIndexDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX11.h"
#include <vector>
#include "GHVertexStream.h"
#include "GHVertexBuffer.h"
#include "GHShaderDX11.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "Render/GHShaderSemantic.h"

GHVBBlitterIndexDX11::GHVBBlitterIndexDX11(unsigned int numIndices, GHRenderDeviceDX11& device, 
										   GHShaderDX11*& activeVS, GHVBUsage::Enum usage,
										   GHEventMgr& eventMgr)
: GHVBBlitterIndex(numIndices)
, mDevice(device)
, mActiveVS(activeVS)
, mUsage(usage)
, mHardwareBuffer(0)
, mMemoryBuffer(0)
, mD3DIED(0)
, mD3DPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
, mEventMgr(eventMgr)
, mMessageListener(*this)
{
	mMemoryBuffer = new unsigned short[numIndices * sizeof(unsigned short)];
	createHardwareBuffer();

	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}
    
GHVBBlitterIndexDX11::~GHVBBlitterIndexDX11(void)
{
	delete [] mMemoryBuffer;
	delete [] mD3DIED;

	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

void GHVBBlitterIndexDX11::prepareVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->prepareVB((int)i, 0);
	}

	if (!mD3DIED) createIED(vb);
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = getInputLayout(mActiveVS);
	if (inputLayout) {
		mDevice.getD3DContext()->IASetInputLayout(inputLayout.Get());
	}
	else
	{
		GHDebugMessage::outputString("Couldn't make input layout in prepareVB! bad bad.");
	}

	if (mHardwareBuffer)
	{
		mDevice.getD3DContext()->IASetIndexBuffer(
			mHardwareBuffer.Get(),
			DXGI_FORMAT_R16_UINT,
			0
			);
	}
    mDevice.getD3DContext()->IASetPrimitiveTopology(mD3DPrimitiveType);
}

void GHVBBlitterIndexDX11::endVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->endVB((int)i);
	}

	mDevice.getD3DContext()->IASetIndexBuffer(
			nullptr,
			DXGI_FORMAT_R16_UINT,
			0
			);
}
    
void GHVBBlitterIndexDX11::blit(void)
{
	if (mActiveIndices)
	{
		mDevice.getD3DContext()->DrawIndexed(mActiveIndices, 0, 0);
	}
}
    
unsigned short* GHVBBlitterIndexDX11::lockWriteBuffer(void)
{
	return mMemoryBuffer;
}
    
void GHVBBlitterIndexDX11::unlockWriteBuffer(void)
{
	if (!mHardwareBuffer) return;

	if (mUsage == GHVBUsage::DYNAMIC) {
		void* voidBuffer = 0;
		D3D11_MAPPED_SUBRESOURCE ms;
		mDevice.getD3DContext()->Map(mHardwareBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		voidBuffer = ms.pData;

		::memcpy(voidBuffer, mMemoryBuffer, mNumIndices * sizeof(unsigned short));

		mDevice.getD3DContext()->Unmap(mHardwareBuffer.Get(), NULL);
	}
	else {
		mDevice.getD3DContext()->UpdateSubresource(
			mHardwareBuffer.Get(),
			0,
			nullptr,
			mMemoryBuffer,
			0,
			0
		);
	}
}
    
const unsigned short* GHVBBlitterIndexDX11::lockReadBuffer(void) const
{
	return mMemoryBuffer;
}
    
void GHVBBlitterIndexDX11::unlockReadBuffer(void) const
{
	// nothing changed in our memory buffer, so don't need to do anything.
}
    
void GHVBBlitterIndexDX11::createHardwareBuffer(void)
{
	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	if (mUsage == GHVBUsage::DYNAMIC) {
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else {
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}
	bufferDesc.ByteWidth       = sizeof(unsigned short) * mNumIndices;
	bufferDesc.BindFlags       = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.MiscFlags       = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = mDevice.getD3DDevice()->CreateBuffer(&bufferDesc, 0, mHardwareBuffer.GetAddressOf());
	if (FAILED(hr)) {
		GHDebugMessage::outputString("Failed to make hardware ib");
	}
}

int GHVBBlitterIndexDX11::applyStreamComponentsToIED(const std::vector<GHVertexStreamFormat::ComponentEntry>& comps, int streamIdx, int localCompIdx)
{
	for (unsigned int compIdx = 0; compIdx < comps.size(); ++compIdx)
	{
		mD3DIED[localCompIdx].InputSlot = streamIdx;
		int semanticIdx = 0;
		mD3DIED[localCompIdx].SemanticName = GHShaderSemantic::getSemanticName(comps[compIdx].mComponent, semanticIdx);
		mD3DIED[localCompIdx].SemanticIndex = semanticIdx;
		mD3DIED[localCompIdx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
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

void GHVBBlitterIndexDX11::createIED(GHVertexBuffer& vb)
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

	mD3DIED = new D3D11_INPUT_ELEMENT_DESC[mInputElementCount];
	int localCompIdx = 0;
	for (unsigned int streamIdx = 0; streamIdx < streams.size(); ++streamIdx)
	{
		const std::vector<GHVertexStreamFormat::ComponentEntry>& comps = streams[streamIdx]->get()->getFormat().getComponents();
		localCompIdx = applyStreamComponentsToIED(comps, streamIdx, localCompIdx);
	}
	localCompIdx = applyStreamComponentsToIED(additionalComps, 0, localCompIdx);
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> GHVBBlitterIndexDX11::getInputLayout(GHShaderDX11* activeVS)
{
	if (!activeVS) {
		return nullptr;
	}

	LayoutMap::iterator findIter = mInputLayouts.find(activeVS);
	if (findIter != mInputLayouts.end()) {
		return findIter->second;
	}
	else {
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = 0;
		const char* data;
		size_t dataLen;
		activeVS->getBytecode(data, dataLen);
		HRESULT ilRes = mDevice.getD3DDevice()->CreateInputLayout(
            mD3DIED,
			mInputElementCount,
            data,
            dataLen,
            inputLayout.GetAddressOf()
            );
		if (FAILED(ilRes)) {
			GHDebugMessage::outputString("Failed to make InputLayout");
		}
		mInputLayouts[activeVS] = inputLayout;
		return inputLayout;
	}
}

void GHVBBlitterIndexDX11::reinit(void)
{
	mHardwareBuffer = nullptr;
	mInputLayouts.clear();
	createHardwareBuffer();
	lockWriteBuffer();
	unlockWriteBuffer();
}
