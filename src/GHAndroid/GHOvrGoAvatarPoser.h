#pragma once

#include "GHOculusAvatarPlatformPoser.h"

class GHOvrGoFrameState;
class GHOvrGoInputState;

class GHOvrGoAvatarPoser : public GHOculusAvatarPlatformPoser
{
public:
	GHOvrGoAvatarPoser(const GHOvrGoFrameState& frameState, const GHOvrGoInputState& inputState);

	virtual void poseAvatar(ovrAvatar* avatar);

private:
	void updateBody(ovrAvatar* avatar);
	void updateHands(ovrAvatar* avatar);

private:
	const GHOvrGoFrameState& mFrameState;
	const GHOvrGoInputState& mInputState;
	double mLastUpdateTime{ 0 };
};

class GHOvrGoAvatarPoserFactory : public GHOculusAvatarPlatformPoserFactory
{
public:
	GHOvrGoAvatarPoserFactory(const GHOvrGoFrameState& frameState, const GHOvrGoInputState& inputState);

	virtual GHOculusAvatarPlatformPoser* createAvatarPoser(void);

private:
	const GHOvrGoFrameState& mFrameState;
	const GHOvrGoInputState& mInputState;
};