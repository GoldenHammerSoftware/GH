#pragma once

#include "GHOculusAvatarPlatformPoser.h"
#include "OVR_CAPI.h"

class GHOculusSystemController;

class GHOculusAvatarPoser : public GHOculusAvatarPlatformPoser
{
public:
	GHOculusAvatarPoser(const GHOculusSystemController& oculusSystem);

	virtual void poseAvatar(ovrAvatar* avatar);

private:
	void convertPoseToAvatarTransform(const ovrPosef& inPose, ovrAvatarTransform& outTrans) const;
	void convertOvrInputToAvatarInput(const ovrAvatarTransform& transform, const ovrInputState& inputState, ovrHandType hand, ovrAvatarHandInputState& outState) const;

private:
	const GHOculusSystemController& mOculusSystem;
	double mLastUpdateTime{ 0 };
};

class GHOculusAvatarPoserFactory : public GHOculusAvatarPlatformPoserFactory
{
public:
	GHOculusAvatarPoserFactory(const GHOculusSystemController& oculusSystem);
	virtual GHOculusAvatarPlatformPoser* createAvatarPoser(void);

private:
	const GHOculusSystemController& mOculusSystem;
};

