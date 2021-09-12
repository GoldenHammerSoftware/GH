// Copyright Golden Hammer Software
#pragma once

#include "GHVertexStreamFormat.h"

class GHVBDescription;
class GHVertexBuffer;
class GHVertexStream;
class GHVBBlitterIndex;

class GHVBFactory
{
public:
    virtual ~GHVBFactory(void) {}
    
    GHVertexBuffer* createVB(const GHVBDescription& desc) const;
    
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const = 0;
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const = 0;
};
