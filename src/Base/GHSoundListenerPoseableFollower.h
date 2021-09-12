#pragma once
#include "GHUtils/GHController.h"

class GHSoundDevice;
class GHInputState;

class GHSoundListenerPoseableFollower : public GHController
{
public:
	GHSoundListenerPoseableFollower(GHSoundDevice& soundDevice, const GHInputState& inputState, unsigned int poseableIndex=0);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	GHSoundDevice& mSoundDevice;
	const GHInputState& mInputState;
	unsigned int mPoseableIndex{ 0 };
};
