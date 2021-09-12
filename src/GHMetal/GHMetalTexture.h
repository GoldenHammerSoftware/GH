#pragma once

#include "GHTexture.h"
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#include "GHMDesc.h"

class GHMetalRenderDevice;

class GHMetalTexture : public GHTexture
{
public:
    GHMetalTexture(id<MTLTexture> metalTexture,
                   GHMetalRenderDevice& device,
                   bool mipmap);
    ~GHMetalTexture(void);
    
    void setBindId(int id);
    void setWrapMode(GHMDesc::WrapMode wrapMode);

    virtual void bind(void);

    virtual bool lockSurface(void** ret, unsigned int& pitch);
    virtual bool unlockSurface(void);
    virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);
    
    // delete any data not required for rendering
    virtual void deleteSourceData(void);
    
    id<MTLTexture> getMetalTexture(void) { return mMetalTexture; }
    
private:
    GHMetalRenderDevice& mDevice;
    id<MTLTexture> mMetalTexture{nil};
    int mBindId{0};
    GHMDesc::WrapMode mWrapMode;
    id<MTLSamplerState> mMetalSampler{nil};
    void* mSourceData{0};
    bool mMipmap{false};
};

