#include "GHVBBlitterIndexDX12.h"

GHVBBlitterIndexDX12::GHVBBlitterIndexDX12(unsigned int numIndices)
	: GHVBBlitterIndex(numIndices)
{
}

GHVBBlitterIndexDX12::~GHVBBlitterIndexDX12(void)
{
}

void GHVBBlitterIndexDX12::prepareVB(GHVertexBuffer& vb)
{
}

void GHVBBlitterIndexDX12::endVB(GHVertexBuffer& vb)
{
}

void GHVBBlitterIndexDX12::blit(void)
{
}

unsigned short* GHVBBlitterIndexDX12::lockWriteBuffer(void)
{
	return 0;
}

void GHVBBlitterIndexDX12::unlockWriteBuffer(void)
{
}

const unsigned short* GHVBBlitterIndexDX12::lockReadBuffer(void) const
{
	return 0;
}

void GHVBBlitterIndexDX12::unlockReadBuffer(void) const
{
}
