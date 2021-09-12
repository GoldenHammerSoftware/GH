#pragma once

#include <vector>
#include <OVR_Avatar.h>

class GHTransform;
class GHOculusSystemController;

class GHOculusAvatarDebugDrawHands
{
public:
	GHOculusAvatarDebugDrawHands(const GHTransform& hmdOrigin);
	~GHOculusAvatarDebugDrawHands(void);

	void drawAvatar(ovrAvatar* avatar);

private:
	void moveFromOvrToWorldSpace(GHTransform& transform);
//	void scalePosition(GHTransform& transform);
	void drawAvatarPart(const ovrAvatarTransform& localTransform, const ovrAvatarSkinnedMeshPose& skinnedPose, const GHTransform& worldTransform);
	unsigned int getNextLineId(void);

private:
	const GHTransform& mHMDOrigin;
	std::vector<unsigned int> mLines;
	unsigned int mNextLine { 0 };
};
