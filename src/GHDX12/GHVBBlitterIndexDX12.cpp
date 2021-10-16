#include "GHVBBlitterIndexDX12.h"

GHVBBlitterIndexDX12::GHVBBlitterIndexDX12(unsigned int numIndices)
	: GHVBBlitterIndex(numIndices)
{
	mMemoryBuffer = new char[numIndices * sizeof(unsigned short)];
}

GHVBBlitterIndexDX12::~GHVBBlitterIndexDX12(void)
{
	delete[] mMemoryBuffer;
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
	return (unsigned short*)mMemoryBuffer;
}

void GHVBBlitterIndexDX12::unlockWriteBuffer(void)
{
}

const unsigned short* GHVBBlitterIndexDX12::lockReadBuffer(void) const
{
	return (unsigned short*)mMemoryBuffer;
}

void GHVBBlitterIndexDX12::unlockReadBuffer(void) const
{
}
