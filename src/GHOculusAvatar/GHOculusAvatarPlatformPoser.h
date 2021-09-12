#pragma once

#include "OVR_Avatar.h"

class GHOculusAvatarPlatformPoser
{
public:
	virtual ~GHOculusAvatarPlatformPoser(void) { }

	virtual void poseAvatar(ovrAvatar* avatar) = 0;
};

class GHOculusAvatarPlatformPoserFactory
{
public:
	virtual ~GHOculusAvatarPlatformPoserFactory(void) { }
	virtual GHOculusAvatarPlatformPoser* createAvatarPoser(void) = 0;
};