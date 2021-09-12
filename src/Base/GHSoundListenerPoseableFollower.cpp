#include "GHSoundListenerPoseableFollower.h"
#include "GHSoundDevice.h"
#include "GHInputState.h"

GHSoundListenerPoseableFollower::GHSoundListenerPoseableFollower(GHSoundDevice& soundDevice, const GHInputState& inputState, unsigned int poseableIndex)
	: mSoundDevice(soundDevice)
	, mInputState(inputState)
	, mPoseableIndex(poseableIndex)
{

}

void GHSoundListenerPoseableFollower::onActivate(void)
{

}

void GHSoundListenerPoseableFollower::onDeactivate(void)
{

}

void GHSoundListenerPoseableFollower::update(void)
{
	auto& poseable = mInputState.getPoseable(mPoseableIndex);

	GHPoint3 pos;
	poseable.mPosition.getTranslate(pos);

	mSoundDevice.setListenerPosition(pos);
	mSoundDevice.setListenerOrientation(poseable.mPosition);
	mSoundDevice.setListenerVelocity(poseable.mLinearVelocity);
}
