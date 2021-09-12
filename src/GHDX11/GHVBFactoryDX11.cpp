// Copyright Golden Hammer Software
#include "GHVBFactoryDX11.h"
#include "GHVertexStreamDX11.h"
#include "GHVBBlitterIndexDX11.h"

GHVBFactoryDX11::GHVBFactoryDX11(GHRenderDeviceDX11& device, GHShaderDX11*& activeVS, GHEventMgr& eventMgr)
: mDevice(device)
, mActiveVS(activeVS)
, mEventMgr(eventMgr)
{
}
    
GHVertexStream* GHVBFactoryDX11::createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
{
	return new GHVertexStreamDX11(format, numVerts, mDevice, usage, mEventMgr);
}

GHVBBlitterIndex* GHVBFactoryDX11::createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const
{
	return new GHVBBlitterIndexDX11(numIndex, mDevice, mActiveVS, usage, mEventMgr);
}
