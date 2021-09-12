#pragma once

#include "GHHMDRenderDevice.h"

class GHHMDRenderDeviceNull : public GHHMDRenderDevice
{
public:
	GHHMDRenderDeviceNull(bool beActive) : mIsActive(beActive) {}

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;

	virtual bool isActive(void) const { return mIsActive; }

	GHRenderTarget* getRenderTarget(RenderTargetType targetType) const { return 0; }

private:
	bool mIsActive;
};
