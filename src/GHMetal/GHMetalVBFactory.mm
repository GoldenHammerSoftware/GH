#import "GHMetalVBFactory.h"
#include "GHMetalVBBlitterIndex.h"
#include "GHMetalVertexStream.h"

GHMetalVBFactory::GHMetalVBFactory(GHMetalRenderDevice& device)
: mDevice(device)
{
}

GHVertexStream* GHMetalVBFactory::createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
{
    GHMetalVertexStream* stream = new GHMetalVertexStream(format, numVerts, mDevice);
    return stream;
}

GHVBBlitterIndex* GHMetalVBFactory::createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const
{
    GHMetalVBBlitterIndex* blitter = new GHMetalVBBlitterIndex(numIndex, mDevice);
    return blitter;
}
