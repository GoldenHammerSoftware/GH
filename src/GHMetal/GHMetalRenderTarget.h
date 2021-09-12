#pragma once

#include "GHRenderTarget.h"
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalRenderDevice;
class GHMetalTexture;

class GHMetalRenderTarget : public GHRenderTarget
{
public:
    GHMetalRenderTarget(const GHRenderTarget::Config& args, GHMetalRenderDevice& device);
    ~GHMetalRenderTarget(void);
    
    virtual void apply(void);
    virtual void remove(void);
    virtual GHTexture* getTexture(void);
    
    void resize(const GHRenderTarget::Config& args);
    
    MTLRenderPassDescriptor* getMtlDescriptor(void) { return mRenderPassDescriptor; }
    
private:
    void initTextures(void);
    GHMetalTexture* initColorTarget(int sampleCount);
    void initDepthTarget(void);
    void initRenderPassDescriptor(void);
    
private:
    GHRenderTarget::Config mConfig;
    GHMetalRenderDevice& mDevice;
    GHMetalTexture* mTexture{0};
    GHMetalTexture* mMsaaTexture{0};
    GHMetalTexture* mDepthTexture{0};
    MTLRenderPassDescriptor* mRenderPassDescriptor{0};
};
