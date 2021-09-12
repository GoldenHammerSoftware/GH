// Copyright Golden Hammer Software
#pragma once

#include "GHVertexStream.h"

// a wrapper stream to pair a different format with an existing stream
// used to send stuff like pos to normal instead without having to duplicate anything.
class GHVertexStreamOverride : public GHVertexStream
{
public:
    GHVertexStreamOverride(GHVertexStreamFormatPtr* format);
    virtual ~GHVertexStreamOverride(void);

    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride);
    virtual void endVB(int streamIdx);
    
    virtual float* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const float* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;
    
    virtual GHVertexStream* clone(void) const;
    
    void setSourceStream(GHVertexStreamPtr* sourceStream);
    
private:
    GHVertexStreamPtr* mSourceStream;
};
