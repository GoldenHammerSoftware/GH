// Copyright Golden Hammer Software
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHTransformNode.h"
#include "GHTransformAnimHelper.h"

GHTransformAnimController::GHTransformAnimController(const GHTimeVal& time)
: mAnimData(0)
, mTarget(0)
, mTime(time)
{
}

GHTransformAnimController::~GHTransformAnimController(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimData, 0);
}


void GHTransformAnimController::setAnimData(GHTransformAnimData* data)
{
    GHRefCounted::changePointer((GHRefCounted*&)mAnimData, data);
}

void GHTransformAnimController::setTarget(GHTransformNode* target)
{
    mTarget = target;
}

void GHTransformAnimController::update(void)
{
	if (!mAnimData) return;
	if (!mTarget) return;
	if (!mFrameChecker.checkIsNewFrame(mTime)) return;
    if (!mAnimBlends.size()) return;
    if (!mAnimBlends[0].mSequence) return;

    GHTransformAnimHelper animHelper;
    animHelper.blendTransform(mAnimBlends, mTime, *mAnimData, mTarget->getLocalTransform().getMatrix());
	mTarget->getLocalTransform().incrementVersion();
}

GHAnimController* GHTransformAnimController::clone(void) const
{
    GHTransformAnimController* ret = new GHTransformAnimController(mTime);
    ret->setAnimData(mAnimData);
    ret->setAnimSet(mAnimSet);
    return ret;
}


