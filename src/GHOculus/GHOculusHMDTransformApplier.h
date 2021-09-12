#pragma once

#include "GHUtils/GHController.h"

class GHTransform;
class GHOculusDX11HMDRenderDevice;

// copies the headset transform from ovr to a destination transform every frame.
class GHOculusHMDTransformApplier : public GHController
{
public:
	GHOculusHMDTransformApplier(GHOculusDX11HMDRenderDevice& hmdDevice, const GHTransform& hmdOrigin, GHTransform& outTrans);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
	GHOculusDX11HMDRenderDevice& mHMDDevice;
	const GHTransform& mHMDOrigin;
	GHTransform& mOutTrans;
};
