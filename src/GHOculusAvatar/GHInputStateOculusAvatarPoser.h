#pragma once

#include "GHOculusAvatarPlatformPoser.h"

class GHInputState;
class GHTimeVal;
class GHTransform;

// A class that sets up the Oculus Avatar poser based on GHInputState
class GHInputStateOculusAvatarPoser : public GHOculusAvatarPlatformPoser
{
public:
	GHInputStateOculusAvatarPoser(const GHTransform& hmdOrigin, const GHInputState& inputState, const GHTimeVal& timeVal);
	virtual void poseAvatar(ovrAvatar* avatar);

private:
	const GHTransform& mHMDOrigin;
	const GHInputState& mInputState;
	const GHTimeVal& mTimeVal;
};

class GHInputStateOculusAvatarPoserFactory : public GHOculusAvatarPlatformPoserFactory
{
public:
	GHInputStateOculusAvatarPoserFactory(const GHTransform& hmdOrigin, const GHInputState& inputState, const GHTimeVal& timeVal);
	virtual ~GHInputStateOculusAvatarPoserFactory(void);
	virtual GHInputStateOculusAvatarPoser* createAvatarPoser(void);

private:
	const GHTransform& mHMDOrigin;
	const GHInputState& mInputState;
	const GHTimeVal& mTimeVal;
};
