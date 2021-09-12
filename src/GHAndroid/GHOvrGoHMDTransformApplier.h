#pragma once

#ifdef GH_OVR_GO

#include "GHUtils/GHController.h"

class GHTransform;
class GHOvrGoFrameState;

// copies the headset transform from ovr to a destination transform every frame.
class GHOvrGoHMDTransformApplier : public GHController
{
public:
	GHOvrGoHMDTransformApplier(const GHOvrGoFrameState& frameState, const GHTransform& hmdOrigin, GHTransform& outTrans);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
	const GHOvrGoFrameState& mFrameState;
	const GHTransform& mHMDOrigin;
	GHTransform& mOutTrans;
};

#endif

