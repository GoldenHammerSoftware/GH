// Copyright Golden Hammer Software
#pragma once

#include "GHVBFactory.h"

class GHRenderDeviceDX11;
class GHShaderDX11;
class GHEventMgr;

class GHVBFactoryDX11 : public GHVBFactory
{
public:
	GHVBFactoryDX11(GHRenderDeviceDX11& device, GHShaderDX11*& activeVS, GHEventMgr& eventMgr);
    
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const;
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const;

private:
	GHRenderDeviceDX11& mDevice;
	GHShaderDX11*& mActiveVS;
	GHEventMgr& mEventMgr;
};
