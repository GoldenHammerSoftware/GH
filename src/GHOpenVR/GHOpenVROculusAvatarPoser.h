#pragma once

#include "GHOculusAvatarPlatformPoser.h"

class GHInputState;

// A class that sets up the Oculus Avatar poser based on OpenVR input
class GHOpenVROculusAvatarPoser : public GHOculusAvatarPlatformPoser
{
public:
	GHOpenVROculusAvatarPoser(GHInputState& inputState);
	virtual void poseAvatar(ovrAvatar* avatar);

private:
	GHInputState& mInputState;
};

class GHOpenVROculusAvatarPoserFactory : public GHOculusAvatarPlatformPoserFactory
{
public:
	GHOpenVROculusAvatarPoserFactory(GHInputState& inputState);

	virtual ~GHOpenVROculusAvatarPoserFactory(void);
	virtual GHOpenVROculusAvatarPoser* createAvatarPoser(void);

private:
	GHInputState& mInputState;
};
