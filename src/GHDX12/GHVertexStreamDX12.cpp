#include "GHVertexStreamDX12.h"

GHVertexStreamDX12::GHVertexStreamDX12(GHVertexStreamFormatPtr* format)
	: GHVertexStream(format)
{
}

GHVertexStreamDX12::~GHVertexStreamDX12(void)
{
}

void GHVertexStreamDX12::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
}

void GHVertexStreamDX12::endVB(int streamIdx)
{
}

float* GHVertexStreamDX12::lockWriteBuffer(void)
{
	return 0;
}

void GHVertexStreamDX12::unlockWriteBuffer(void)
{
}

const float* GHVertexStreamDX12::lockReadBuffer(void) const
{
	return 0;
}

void GHVertexStreamDX12::unlockReadBuffer(void) const
{
}

GHVertexStream* GHVertexStreamDX12::clone(void) const
{
	return 0;
}
