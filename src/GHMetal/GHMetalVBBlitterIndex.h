#pragma once

#include "GHVBBlitter.h"
#include "GHMetalRenderDevice.h"
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalVBBlitterIndex : public GHVBBlitterIndex
{
public:
    GHMetalVBBlitterIndex(unsigned int numIndices, GHMetalRenderDevice& device);
    ~GHMetalVBBlitterIndex(void);
    
    virtual unsigned short* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;
    
    virtual const unsigned short* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;

    virtual void prepareVB(GHVertexBuffer& vb) override;
    virtual void endVB(GHVertexBuffer& vb) override;
    virtual void blit(void) override;
   
    MTLVertexDescriptor* getVertexDescriptor(void) { return mVertexDescriptor; }

private:
    GHMetalRenderDevice& mDevice;
    id<MTLBuffer> mMetalBuffer;
    MTLVertexDescriptor* mVertexDescriptor{0};
};

