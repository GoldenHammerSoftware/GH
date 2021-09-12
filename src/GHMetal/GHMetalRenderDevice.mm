#include "GHMetalRenderDevice.h"
#import <MetalKit/MetalKit.h>
#include "GHMetalRenderTarget.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHMutex.h"
#include "GHMessage.h"
#include "GHMessageTypes.h"
#include "GHEventMgr.h"
#include "GHScreenInfo.h"
#include "GHCamera.h"
#include "GHMath/GHRect.h"

@interface GHMetalViewCallback : NSObject<MTKViewDelegate>
{
    @public GHMetalRenderDevice* mParent;
}
- (void)drawInMTKView:(nonnull MTKView *)view;
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size;
@end
@implementation GHMetalViewCallback

/// Called whenever the view needs to render a frame.
- (void)drawInMTKView:(nonnull MTKView *)view
{
    mParent->handleBlit();
}

/// Called whenever view changes orientation or is resized
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    mParent->handleDrawableSizeChange(size.width, size.height);
}

@end

GHMetalRenderDevice::GHMetalRenderDevice(GHiOSViewMetal& view, const GHThreadFactory& threadFactory, GHEventMgr& systemEventMgr, GHScreenInfo& screenInfo)
: mView(view)
, mSystemEventMgr(systemEventMgr)
, mScreenInfo(screenInfo)
{
    mMutex = threadFactory.createMutex();
    mMetalDevice = MTLCreateSystemDefaultDevice();
    (&mView).device = mMetalDevice;
    
    GHMetalViewCallback* delegate = [[GHMetalViewCallback alloc] init];
    mViewDelegate = delegate;
    delegate->mParent = this;
    (&mView).delegate = delegate;
    
    // set us up for manual draws triggered by endFrame.
    (&mView).enableSetNeedsDisplay = NO;
    (&mView).paused = YES;
    (&mView).depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    
    if ([mMetalDevice supportsTextureSampleCount:4])
    {
        mMsaaSampleCount = 4;
    }
    (&mView).sampleCount = mMsaaSampleCount;
    
    mCommandQueue = [mMetalDevice newCommandQueue];

    (&mView).contentScaleFactor = 1.0f;
    
    // Initialize the renderer with the view size.
    [delegate mtkView:(&mView) drawableSizeWillChange:(&mView).drawableSize];
}

GHMetalRenderDevice::~GHMetalRenderDevice(void)
{
    delete mMutex;
}

bool GHMetalRenderDevice::beginFrame(void)
{
    mMutex->acquire();
    mCommandBuffer = [mCommandQueue commandBuffer];

    // we won't have a commandEncoder until beginRenderPass is called.
    // todo: clear color
    beginRenderPass(0, GHPoint4(0,0,0,0), true);
    
    return true;
}

void GHMetalRenderDevice::endFrame(void)
{
    // tie off the main render pass if needed.
    endRenderPass();

    id<MTLDrawable> drawable = (&mView).currentDrawable;
    [mCommandBuffer presentDrawable:drawable];
    
    mMutex->release();
    // this should trigger drawInMTKView
    [(&mView) draw];
}

void GHMetalRenderDevice::setClearColor(const GHPoint4& color)
{
    (&mView).clearColor = MTLClearColorMake(color[0], color[1], color[2], color[3]);
}

void GHMetalRenderDevice::applyViewInfo(const GHViewInfo& viewInfo)
{
    mActiveViewInfo = viewInfo;
}

void GHMetalRenderDevice::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
    GHViewInfo::ViewTransforms& deviceTransforms,
    const GHCamera& camera, bool isRenderToTexture) const
{
    deviceTransforms = engineTransforms;

    deviceTransforms.mPlatformGUITrans = GHTransform(
        2.0, 0.0, 0.0, 0.0,
        0.0, -2.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        -1.0, 1.0, 0.0, 1.0
    );
    
    // metal device transforms are different!
    // http://metashapes.com/blog/opengl-metal-projection-matrix-problem/
    if (camera.getIsOrtho())
    {
        // I think our orthof transform might be bugged.
        // see hackForOrthofView.
        // The difference is tz.
        // Making a slightly different version here.
        
        GHRect<float, 2> frustumRect;
        float halfCamWidth = camera.getOrthoWidth()/2.0f;
        float halfCamHeight = camera.getOrthoHeight()/2.0f;
        frustumRect.mMin.setCoords(-halfCamWidth, -halfCamHeight);
        frustumRect.mMax.setCoords(halfCamWidth, halfCamHeight);
        
        const float right = frustumRect.mMax[0];
        const float left = frustumRect.mMin[0];
        const float top = frustumRect.mMax[0];
        const float bottom = frustumRect.mMin[0];
        const float zNear = camera.getNearClip();
        const float zFar = camera.getFarClip();
        
        const float x = 2.0f / (right - left);
        const float y = 2.0f / (top - bottom);
        const float z = -1.0f / (zFar - zNear);
        const float tx = -(right + left) / (right - left);
        const float ty = -(top + bottom) / (top - bottom);
        const float tz = -zNear / (zFar - zNear);
        
        deviceTransforms.mProjectionTransform[ 0] = x;
        deviceTransforms.mProjectionTransform[ 5] = y;
        deviceTransforms.mProjectionTransform[10] = z;
        deviceTransforms.mProjectionTransform[12] = tx;
        deviceTransforms.mProjectionTransform[13] = ty;
        deviceTransforms.mProjectionTransform[14] = tz;
        deviceTransforms.mProjectionTransform[15] = 1.0f;
        deviceTransforms.mProjectionTransform[ 1] = deviceTransforms.mProjectionTransform[ 2] = deviceTransforms.mProjectionTransform[ 3] =
        deviceTransforms.mProjectionTransform[ 4] = deviceTransforms.mProjectionTransform[ 6] = deviceTransforms.mProjectionTransform[ 7] =
        deviceTransforms.mProjectionTransform[ 8] = deviceTransforms.mProjectionTransform[ 9] = deviceTransforms.mProjectionTransform[11] = 0.0f;
        
        deviceTransforms.mProjectionTransform.incrementVersion();
    }
    else
    {
        deviceTransforms.mProjectionTransform[10] *= 0.5f;
        deviceTransforms.mProjectionTransform[14] *= 0.5f;
    }
    deviceTransforms.calcDerivedTransforms(camera);
}

const GHViewInfo& GHMetalRenderDevice::getViewInfo(void) const
{
    return mActiveViewInfo;
}

void GHMetalRenderDevice::handleGraphicsQualityChange(void)
{
}

GHTexture* GHMetalRenderDevice::resolveBackbuffer(void)
{
    return 0;
}

void GHMetalRenderDevice::handleBlit(void)
{
    mMutex->acquire();
    endEncoders();
    if (mCommandBuffer)
    {
        [mCommandBuffer commit];
        // shouldn't be necessary and makes things slow.
        //[mCommandBuffer waitUntilCompleted];
        mCommandBuffer = nil;
    }
    mMutex->release();
}

void GHMetalRenderDevice::handleDrawableSizeChange(int width, int height)
{
    mScreenInfo.setSize(GHPoint2i(width, height));
    GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
    mSystemEventMgr.handleMessage(resizeMessage);
}

void GHMetalRenderDevice::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
    endEncoders();

    MTLRenderPassDescriptor* renderPassDescriptor = (&mView).currentRenderPassDescriptor;
    if (optionalTarget)
    {
        mActiveRenderTarget = optionalTarget;
        mActiveRenderTarget->apply();
        renderPassDescriptor = ((GHMetalRenderTarget*)optionalTarget)->getMtlDescriptor();
    }
    assert(renderPassDescriptor);

    if (clearBuffers)
    {
        renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    }
    else
    {
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionLoad;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
    }
    if (!optionalTarget)
    {
        // probably don't need to always store.
        MTLStoreAction store = MTLStoreActionStore;
        if (mMsaaSampleCount > 1) store = MTLStoreActionStoreAndMultisampleResolve;
        renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].storeAction = store;
    }
    setClearColor(clearColor);
    renderPassDescriptor.colorAttachments[0].clearColor = (&mView).clearColor;
    
    mCommandEncoder = [mCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    assert(mCommandEncoder);
}

void GHMetalRenderDevice::endRenderPass(void)
{
    // doing this here might be a waste.
    endEncoders();
    if (mActiveRenderTarget)
    {
        mActiveRenderTarget->remove();
        mActiveRenderTarget = 0;
    }
}

id<MTLBlitCommandEncoder> GHMetalRenderDevice::beginBlitEncoder(void)
{
    endEncoders();
    mBlitEncoder = [mCommandBuffer blitCommandEncoder];
    return mBlitEncoder;
}

void GHMetalRenderDevice::endBlitEncoder(void)
{
    // doing this here might be a waste.
    endEncoders();
}

void GHMetalRenderDevice::beginMaterial(void)
{
    if (!mCommandEncoder)
    {
        beginRenderPass(0, GHPoint4(0,0,0,0), false);
    }
}

void GHMetalRenderDevice::endEncoders(void)
{
    if (mCommandEncoder)
    {
        [mCommandEncoder endEncoding];
        mCommandEncoder = nil;
    }
    if (mBlitEncoder)
    {
        [mBlitEncoder endEncoding];
        mBlitEncoder = nil;
    }
}
