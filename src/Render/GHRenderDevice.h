// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHViewInfo.h"

class GHTransform;
class GHHMDRenderDevice;
class GHTexture;
class GHRenderTarget;

// base class for a render system.
class GHRenderDevice
{
public:
	virtual ~GHRenderDevice(void);

    // returns false if we're not ready to draw a frame for whatever reason.
	virtual bool beginFrame(void) = 0;
	virtual void endFrame(void) = 0;

	virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers) = 0;
	virtual void endRenderPass(void) = 0;
    
    virtual void applyViewInfo(const GHViewInfo& viewInfo) = 0;
	virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
		GHViewInfo::ViewTransforms& deviceTransforms,
		const GHCamera& camera, bool isRenderToTexture) const = 0;
	virtual const GHViewInfo& getViewInfo(void) const = 0;

	virtual void handleGraphicsQualityChange(void) = 0;

	virtual GHTexture* resolveBackbuffer(void) = 0;

	void setHMDDevice(GHHMDRenderDevice* hmd);
	GHHMDRenderDevice* getHMDDevice(void) const { return mHMDDevice; }
	void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;
	// is stereo means we are currently rendering a stereo pass.
	void setIsStereo(bool val);
	bool getIsStereo(void) const;

protected:
	bool mIsStereo{ false };
	GHHMDRenderDevice* mHMDDevice{ nullptr };
};
