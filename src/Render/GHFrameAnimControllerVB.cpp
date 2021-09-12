// Copyright Golden Hammer Software
#include "GHFrameAnimControllerVB.h"
#include "GHVBAnimData.h"

GHFrameAnimControllerVB::GHFrameAnimControllerVB(void)
: mAnimData(0)
, mTarget(0)
{
}

GHFrameAnimControllerVB::~GHFrameAnimControllerVB(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimData, 0);
}

void GHFrameAnimControllerVB::setTarget(GHGeometry* target)
{
    mTarget = target;
}

void GHFrameAnimControllerVB::setAnimData(GHVBAnimData* data)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimData, data);
}

