// Copyright Golden Hammer Software
#include "GHVertexStreamDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHEventMgr.h"

GHVertexStreamDX11::GHVertexStreamDX11(GHVertexStreamFormatPtr* format, unsigned int numVerts, 
									   GHRenderDeviceDX11& device, GHVBUsage::Enum usage,
									   GHEventMgr& eventMgr)
: GHVertexStream(format)
, mMemoryBuffer(0)
, mDevice(device)
, mHardwareBuffer(0)
, mUsage(usage)
, mMessageListener(*this)
, mEventMgr(eventMgr)
{
	mNumVerts = numVerts;
	size_t bufferSize = mFormat->get()->getVertexSize() * mNumVerts * sizeof(float);
	mMemoryBuffer = new char[bufferSize];
	createHardwareBuffer();

	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

GHVertexStreamDX11::~GHVertexStreamDX11(void)
{
	delete mMemoryBuffer;
	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

void GHVertexStreamDX11::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
	UINT stride = mFormat->get()->getVertexSize()*sizeof(float);
    UINT offset = 0;
    mDevice.getD3DContext()->IASetVertexBuffers(
        streamIdx,
        1,
        mHardwareBuffer.GetAddressOf(),
        &stride,
        &offset
        );
}

void GHVertexStreamDX11::endVB(int streamIdx)
{
}

float* GHVertexStreamDX11::lockWriteBuffer(void)
{
	return (float*)mMemoryBuffer;
}

void GHVertexStreamDX11::unlockWriteBuffer(void)
{
	if (!mHardwareBuffer) return;

	if (mUsage == GHVBUsage::DYNAMIC) 
	{
		void* voidBuffer = 0;
		D3D11_MAPPED_SUBRESOURCE ms;
		mDevice.getD3DContext()->Map(mHardwareBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); 
		voidBuffer = ms.pData;

		::memcpy(voidBuffer, mMemoryBuffer, mNumVerts * mFormat->get()->getVertexSize()*sizeof(float));

		mDevice.getD3DContext()->Unmap(mHardwareBuffer.Get(), NULL);
	}
	else
	{
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

const float* GHVertexStreamDX11::lockReadBuffer(void) const
{
	return (float*)mMemoryBuffer;
}

void GHVertexStreamDX11::unlockReadBuffer(void) const  
{
}

GHVertexStream* GHVertexStreamDX11::clone(void) const
{
	GHVertexStreamDX11* ret = new GHVertexStreamDX11(mFormat, mNumVerts, mDevice, mUsage, mEventMgr);

	float* retBuffer = ret->lockWriteBuffer();
	memcpy(retBuffer, mMemoryBuffer, mFormat->get()->getVertexSize()*mNumVerts*sizeof(float));
	ret->unlockWriteBuffer();
	return ret;
}

void GHVertexStreamDX11::createHardwareBuffer(void)
{
	D3D11_BUFFER_DESC bufferDesc;
	if (mUsage == GHVBUsage::DYNAMIC) {
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else {
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}
	bufferDesc.ByteWidth        = mNumVerts * mFormat->get()->getVertexSize()*sizeof(float);
	bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.MiscFlags        = 0;

	HRESULT hr = mDevice.getD3DDevice()->CreateBuffer(&bufferDesc, 0, mHardwareBuffer.GetAddressOf());
	if (FAILED(hr)) {
		GHDebugMessage::outputString("Failed to make hardware vb");
	}
}

void GHVertexStreamDX11::reinit(void)
{
	mHardwareBuffer = nullptr;
	createHardwareBuffer();
	lockWriteBuffer();
	unlockWriteBuffer();
}
