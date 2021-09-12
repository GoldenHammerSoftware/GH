// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHTransformNode;
class GHTransform;

// copies the position from a transform node to a target transform
class GHTransformNodeFollower : public GHController
{
public:
	GHTransformNodeFollower(GHTransform& dest, GHTransformNode& src);

	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
	GHTransform& mDest;
	GHTransformNode& mSrc;
};
