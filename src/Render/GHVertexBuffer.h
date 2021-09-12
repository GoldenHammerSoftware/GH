// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHPlatform/GHRefCounted.h"
#include "GHVertexStream.h"
#include "GHVBDescription.h"
#include "GHVBBlitter.h"
#include "GHPlatform/GHRefCounted.h"

class GHVertexBuffer
{
public:
    GHVertexBuffer(void);
    ~GHVertexBuffer(void);
    
    void prepareVB(void);
    void endVB(void);
    void blit(void);
    
    void replaceStream(unsigned int streamId, GHVertexStreamPtr* stream);
    GHVertexStreamPtr* getStream(unsigned int streamId);
    const std::vector<GHVertexStreamPtr*>& getStreams(void) const { return mStreams; }
    
    void replaceBlitter(GHVBBlitterPtr* blitter);
    GHVBBlitterPtr* getBlitter(void) { return mBlitter; }
    const GHVBBlitterPtr* getBlitter(void) const { return mBlitter; }
    
    GHVertexBuffer* clone(void);
    
private:
    std::vector<GHVertexStreamPtr*> mStreams;
    GHVBBlitterPtr* mBlitter;
};

typedef GHRefCountedType<GHVertexBuffer> GHVertexBufferPtr;
