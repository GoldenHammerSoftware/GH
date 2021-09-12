// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVBFactory.h"

class GHVBFactoryOGL : public GHVBFactory
{
public:
    virtual GHVertexStream* createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const;
    virtual GHVBBlitterIndex* createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const;
};
