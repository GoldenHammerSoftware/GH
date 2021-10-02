// Copyright Golden Hammer Software
#pragma once

#include "Render/GHVertexStream.h"

class GHNullVertexStream : public GHVertexStream
{
public:
    GHNullVertexStream(GHVertexStreamFormatPtr* format, int numVerts)
    : GHVertexStream(format)
    , mBuffer(new float[numVerts*format->get()->getVertexSize()])
    {
        mNumVerts = numVerts;
    }
    
    ~GHNullVertexStream(void) { delete [] mBuffer; }
    
	virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride) {}
	virtual void endVB(int streamIdx) {}
    
    virtual float* lockWriteBuffer(void) { return mBuffer; }
    virtual void unlockWriteBuffer(void) {}
    
    virtual const float* lockReadBuffer(void) const { return mBuffer; }
    virtual void unlockReadBuffer(void) const {}
    
    virtual GHVertexStream* clone(void) const { return new GHNullVertexStream(mFormat, mNumVerts); }
    
private:
    float* mBuffer;
};
