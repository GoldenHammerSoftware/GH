// Copyright Golden Hammer Software
#include "GHTransformNodeFollower.h"
#include "GHMath/GHTransform.h"
#include "GHTransformNode.h"
#include "GHPlatform/GHDebugMessage.h"

GHTransformNodeFollower::GHTransformNodeFollower(GHTransform& dest, GHTransformNode& src)
: mDest(dest)
, mSrc(src)
{
}

void GHTransformNodeFollower::update(void)
{
	GHPoint3 pos;
	mSrc.getTransform().getTranslate(pos);
	mDest.setTranslate(pos);
//	GHDebugMessage::outputString("pos %f %f %f version %d", pos[0], pos[1], pos[2], mSrc.getTransform().getVersion());
}
