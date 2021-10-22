#include "GHVertexStreamDX12.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"

GHVertexStreamDX12::GHVertexStreamDX12(GHRenderDeviceDX12& device, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage)
	: GHVertexStream(format)
	, mDevice(device)
	// all dynamic until we can test static in isolation.
	, mBuffer(device, format->get()->getVertexSize() * numVerts * sizeof(float), GHVBUsage::DYNAMIC /*usage*/)
{
	mNumVerts = numVerts;

	mDXView.BufferLocation = mBuffer.getDXBuffer()->GetGPUVirtualAddress();
	mDXView.StrideInBytes = mFormat->get()->getVertexSize();
	mDXView.SizeInBytes = format->get()->getVertexSize() * numVerts;
}

GHVertexStreamDX12::~GHVertexStreamDX12(void)
{
}

void GHVertexStreamDX12::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
	mDevice.getRenderCommandList()->IASetVertexBuffers(streamIdx, 1, &mDXView);
}

void GHVertexStreamDX12::endVB(int streamIdx)
{
}

float* GHVertexStreamDX12::lockWriteBuffer(void)
{
	return (float*)mBuffer.lockWriteBuffer();
}

void GHVertexStreamDX12::unlockWriteBuffer(void)
{
	mBuffer.unlockWriteBuffer();
}

const float* GHVertexStreamDX12::lockReadBuffer(void) const
{
	return (float*)mBuffer.lockReadBuffer();
}

void GHVertexStreamDX12::unlockReadBuffer(void) const
{
	mBuffer.unlockReadBuffer();
}

GHVertexStream* GHVertexStreamDX12::clone(void) const
{
	GHVertexStreamDX12* ret = new GHVertexStreamDX12(mDevice, mFormat, mNumVerts, mBuffer.getUsage());

	float* retBuffer = ret->lockWriteBuffer();
	const void* src = mBuffer.lockReadBuffer();
	memcpy(retBuffer, src, mFormat->get()->getVertexSize() * mNumVerts * sizeof(float));
	mBuffer.unlockReadBuffer();
	ret->unlockWriteBuffer();
	return ret;
}

