// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderDevice.h"
#include "GHViewInfo.h"

class GHOGLContext;
class GHMutex;

class GHRenderDeviceOGL : public GHRenderDevice
{
public:
    GHRenderDeviceOGL(GHOGLContext& context, GHMutex& renderMutex);
    
	virtual bool beginFrame(void);
	virtual void endFrame(void);
    
    virtual void setClearColor(const GHPoint4& color);
	virtual void clearBuffers(void);
	virtual void clearZBuffer(void);
	virtual void clearBackBuffer(void);
    
    virtual void applyViewInfo(const GHViewInfo& viewInfo);
    virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
                                            GHViewInfo::ViewTransforms& deviceTransforms,
                                            const GHCamera& camera, bool isRenderToTexture) const
    {
        deviceTransforms = engineTransforms;
    }

    virtual const GHViewInfo& getViewInfo(void) const { return mActiveViewInfo; }

	virtual void setGraphicsQuality(int val) {}

private:
    GHOGLContext& mContext;
    // the values last passed to applyViewInfo
    GHViewInfo mActiveViewInfo;
    // Some operations (texture loading) can not happen mid-frame.
    // We claim a mutex in between beginFrame() and endFrame() calls.
    GHMutex& mRenderMutex;
};
