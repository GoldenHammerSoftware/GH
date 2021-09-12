// Copyright Golden Hammer Software
#pragma once

#include "GHRenderDevice.h"
#include "GHViewInfo.h"
#include "GHHMDRenderDevice.h"

class GHNullRenderDevice : public GHRenderDevice
{
public:
	virtual bool beginFrame(void) { return true; }
	virtual void endFrame(void) {}

	virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers) {}
	virtual void endRenderPass(void) {}

	virtual void applyViewInfo(const GHViewInfo& viewInfo) {}
	virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
		GHViewInfo::ViewTransforms& deviceTransforms,
		const GHCamera& camera, bool isRenderToTexture) const
	{
		deviceTransforms = engineTransforms;
	}

	virtual void modifyOrthoProj(GHTransform& proj) const {}
	virtual const GHViewInfo& getViewInfo(void) const { return mViewInfo; }

	virtual void handleGraphicsQualityChange(void) {}

	virtual GHTexture* resolveBackbuffer(void) { return nullptr; }

private:
    GHViewInfo mViewInfo;
};
