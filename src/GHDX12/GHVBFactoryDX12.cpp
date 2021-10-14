#include "GHVBFactoryDX12.h"
#include "GHVertexStreamDX12.h"
#include "GHVBBlitterIndexDX12.h"

GHVBFactoryDX12::GHVBFactoryDX12(GHRenderDeviceDX12& device)
	: mDevice(device)
{
}

GHVertexStream* GHVBFactoryDX12::createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
{
	return new GHVertexStreamDX12(mDevice, format, numVerts, usage);
}

GHVBBlitterIndex* GHVBFactoryDX12::createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const
{
	return new GHVBBlitterIndexDX12(numIndex);
}
