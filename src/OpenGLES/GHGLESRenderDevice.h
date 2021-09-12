// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderDevice.h"
#include "GHViewInfo.h"

class GHGLESContext;
class GHMutex;
class GHGLESStateCache;

class GHGLESRenderDevice : public GHRenderDevice
{
public:
    GHGLESRenderDevice(GHGLESContext& context, GHMutex& renderMutex, GHGLESStateCache& stateCache, const int& graphicsQuality);

    virtual bool beginFrame(void);
    virtual void endFrame(void);

    virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers);
    virtual void endRenderPass(void);

    virtual void applyViewInfo(const GHViewInfo& viewInfo);
    virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
        GHViewInfo::ViewTransforms& deviceTransforms,
        const GHCamera& camera, bool isRenderToTexture) const;
    virtual const GHViewInfo& getViewInfo(void) const { return mActiveViewInfo; }

    virtual void handleGraphicsQualityChange(void);
    const int& getGraphicsQuality(void) const { return mGraphicsQuality; }

    virtual GHTexture* resolveBackbuffer(void) { return 0; /*todo*/ }

private:
    virtual void setClearColor(const GHPoint4& color);
    virtual void clearBuffers(void);
    virtual void clearZBuffer(void);
    virtual void clearBackBuffer(void);

private:
    GHGLESContext& mContext;
    GHGLESStateCache& mStateCache;
    const int& mGraphicsQuality;

    // the values last passed to applyViewInfo
    GHViewInfo mActiveViewInfo;
    // Some operations (texture loading) can not happen mid-frame.
    // We claim a mutex in between beginFrame() and endFrame() calls.
    GHMutex& mRenderMutex;

    GHRenderTarget* mRenderTarget{ 0 };
};
