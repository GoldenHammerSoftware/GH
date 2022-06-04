#pragma once

#include "GHUtils/GHController.h"

class GHTransform;
class GHOculusHMDRenderDevice;

// copies the headset transform from ovr to a destination transform every frame.
class GHOculusHMDTransformApplier : public GHController
{
public:
	GHOculusHMDTransformApplier(GHOculusHMDRenderDevice& hmdDevice, const GHTransform& hmdOrigin, GHTransform& outTrans);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
	GHOculusHMDRenderDevice& mHMDDevice;
	const GHTransform& mHMDOrigin;
	GHTransform& mOutTrans;
};
