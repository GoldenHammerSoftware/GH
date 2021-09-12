// Copyright Golden Hammer Software
#pragma once

#include "GHVBBlitter.h"

class GHNullVBBlitterIndex : public GHVBBlitterIndex
{
public:
    GHNullVBBlitterIndex(unsigned int numIndices)
    : GHVBBlitterIndex(numIndices)
    , mBuffer(new unsigned short[numIndices])
    {
    }
    ~GHNullVBBlitterIndex(void)
    {
        delete [] mBuffer;
    }
    
    virtual void prepareVB(GHVertexBuffer& vb) {}
    virtual void endVB(GHVertexBuffer& vb) {}
    virtual void blit(void) {}
    
    virtual unsigned short* lockWriteBuffer(void) { return mBuffer; }
    virtual void unlockWriteBuffer(void) {}
    
    virtual const unsigned short* lockReadBuffer(void) const { return mBuffer; }
    virtual void unlockReadBuffer(void) const {}
    
private:
    unsigned short* mBuffer;
};
