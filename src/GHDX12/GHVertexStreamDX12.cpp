#include "GHVertexStreamDX12.h"
#include "GHRenderDeviceDX12.h"

GHVertexStreamDX12::GHVertexStreamDX12(GHRenderDeviceDX12& device, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage)
	: GHVertexStream(format)
	, mDevice(device)
	, mUsage(usage)
{
	mNumVerts = numVerts;
	size_t bufferSize = mFormat->get()->getVertexSize() * mNumVerts * sizeof(float);
	mMemoryBuffer = new char[bufferSize];
}

GHVertexStreamDX12::~GHVertexStreamDX12(void)
{
	delete[] mMemoryBuffer;
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
