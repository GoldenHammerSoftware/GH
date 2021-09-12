// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include "GHVertexStreamFormat.h"

class GHVertexStream
{
public:
    GHVertexStream(GHVertexStreamFormatPtr* format) : mFormat(format), mNumVerts(0)
    {
        mFormat->acquire();
    }
    virtual ~GHVertexStream(void) 
    {
        mFormat->release();
    }
    
    // set up the vertex components for use by the gpu.
    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride) = 0;
    virtual void endVB(int streamIdx) = 0;
    
    virtual float* lockWriteBuffer(void) = 0;
    virtual void unlockWriteBuffer(void) = 0;
    
    virtual const float* lockReadBuffer(void) const = 0;
    virtual void unlockReadBuffer(void) const = 0;
    
    virtual GHVertexStream* clone(void) const = 0;
    
    unsigned int getNumVerts(void) const { return mNumVerts; }
    
    const GHVertexStreamFormat& getFormat(void) const { return *(mFormat->get()); }
    
protected:
    GHVertexStreamFormatPtr* mFormat;
    unsigned int mNumVerts;
};

typedef GHRefCountedType<GHVertexStream> GHVertexStreamPtr;
