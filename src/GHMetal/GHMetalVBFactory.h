#pragma once

#include "GHVBFactory.h"

class GHMetalRenderDevice;

class GHMetalVBFactory : public GHVBFactory
{
public:
    GHMetalVBFactory(GHMetalRenderDevice& device);
    
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const override;
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const override;
    
private:
    GHMetalRenderDevice& mDevice;
};
