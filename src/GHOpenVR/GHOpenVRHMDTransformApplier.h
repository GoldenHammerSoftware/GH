#pragma once

#include "GHUtils/GHController.h"

class GHOpenVRInputHandler;
class GHTransform;

class GHOpenVRHMDTransformApplier : public GHController
{
public:
	GHOpenVRHMDTransformApplier(const GHOpenVRInputHandler& inputSource, const GHTransform& hmdOrigin, GHTransform& outTrans);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
	const GHOpenVRInputHandler& mInputSource;
	const GHTransform& mHMDOrigin;
	GHTransform& mOutTrans;
};
