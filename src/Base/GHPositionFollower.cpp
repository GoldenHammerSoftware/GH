// Copyright Golden Hammer Software
#include "GHPositionFollower.h"
#include "GHMath/GHTransform.h"

GHPositionFollower::GHPositionFollower(GHTransform& dest, const GHTransform& src)
: mDest(dest)
, mSrc(src)
{
}

void GHPositionFollower::update(void)
{
    GHPoint3 pos;
    mSrc.getTranslate(pos);
    mDest.setTranslate(pos);
}
