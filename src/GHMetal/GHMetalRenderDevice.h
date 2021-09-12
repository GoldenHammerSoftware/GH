#pragma once

#include "GHRenderDevice.h"
#include "GHiOSViewMetal.h"
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHThreadFactory;
class GHMutex;
class GHEventMgr;
class GHScreenInfo;

class GHMetalRenderDevice : public GHRenderDevice
{
public:
    GHMetalRenderDevice(GHiOSViewMetal& view, const GHThreadFactory& threadFactory, GHEventMgr& systemEventMgr, GHScreenInfo& screenInfo);
    ~GHMetalRenderDevice(void);
    
    virtual bool beginFrame(void) override;
    virtual void endFrame(void) override;

    virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers) override;
    virtual void endRenderPass(void) override;
    
    id<MTLBlitCommandEncoder> beginBlitEncoder(void);
    void endBlitEncoder(void);
    
    virtual void applyViewInfo(const GHViewInfo& viewInfo) override;
    virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
        GHViewInfo::ViewTransforms& deviceTransforms,
        const GHCamera& camera, bool isRenderToTexture) const override;
    virtual const GHViewInfo& getViewInfo(void) const override;

    virtual void handleGraphicsQualityChange(void) override;

    virtual GHTexture* resolveBackbuffer(void) override;
    
    // do the actual blit.
    // this is only meant to be called by GHMetalViewCallback which is not visible here.
    void handleBlit(void);
    void handleDrawableSizeChange(int width, int height);

    id<MTLDevice> getMetalDevice(void) { return mMetalDevice; }
    id<MTLRenderCommandEncoder> getRenderCommandEncoder(void) { assert(mCommandEncoder); return mCommandEncoder; }
    id<MTLCommandQueue> getCommandQueue(void) { return mCommandQueue; }
    GHiOSViewMetal& getiOSView(void) { return mView; }
    
    // a callback to do anything lazy before drawing a material
    void beginMaterial(void);
    
    int getMsaaSampleCount(void) const { return mMsaaSampleCount; }
    
private:
    void setClearColor(const GHPoint4& color);
    void endEncoders(void);
    
private:
    // the values last passed to applyViewInfo
    GHViewInfo mActiveViewInfo;
    GHiOSViewMetal& mView;
    NSObject* mViewDelegate{0};
    GHMutex* mMutex{0};
    GHEventMgr& mSystemEventMgr;
    GHScreenInfo& mScreenInfo;
    GHRenderTarget* mActiveRenderTarget{0};
    int mMsaaSampleCount{1};
    
    id<MTLDevice> mMetalDevice{nil};
    id<MTLCommandQueue> mCommandQueue{nil};
    // command buffer for the main render pass.  only valid for one frame.
    id<MTLCommandBuffer> mCommandBuffer{nil};
    // encoder for the main render pass.  might need to move this into GHRenderPass.
    id<MTLRenderCommandEncoder> mCommandEncoder{nil};
    // encoder for texture copies
    id<MTLBlitCommandEncoder> mBlitEncoder{nil};
};

