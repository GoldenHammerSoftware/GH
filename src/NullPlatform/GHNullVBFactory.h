// Copyright Golden Hammer Software
#pragma once

#include "Render/GHVBFactory.h"
#include "GHNullVBBlitterIndex.h"
#include "GHNullVertexStream.h"

class GHNullVBFactory : public GHVBFactory
{
public:  
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
    {
        return new GHNullVertexStream(format, numVerts);
    }
    
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const 
    {
        return new GHNullVBBlitterIndex(numIndex);
    }
};
