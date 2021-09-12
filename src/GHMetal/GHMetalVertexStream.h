#pragma once

#include "GHVertexStream.h"
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalRenderDevice;

class GHMetalVertexStream : public GHVertexStream
{
public:
    GHMetalVertexStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHMetalRenderDevice& device);
    ~GHMetalVertexStream(void);
    
    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride) override;
    virtual void endVB(int streamIdx) override;
    
    virtual float* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;
    
    virtual const float* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;
    
    virtual GHVertexStream* clone(void) const override;
    
private:
    GHMetalRenderDevice& mDevice;
    id<MTLBuffer> mMetalBuffer;
};

