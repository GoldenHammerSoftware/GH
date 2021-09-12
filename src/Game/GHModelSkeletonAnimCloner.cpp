// Copyright Golden Hammer Software
#include "GHModelSkeletonAnimCloner.h"
#include "GHSkeletonAnimController.h"
#include "GHModel.h"

GHModelSkeletonAnimCloner::GHModelSkeletonAnimCloner(GHSkeletonAnimController& con)
: mCon(con)
{
}

void GHModelSkeletonAnimCloner::clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                                      GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes)
{
    assert(mCon.getAnimData().size() == cloneNodes.size());
    GHSkeletonAnimController* retCon = (GHSkeletonAnimController*)mCon.clone();
    retCon->setTarget(cloneNodes, mCon.getAnimData());
    cloneModel.addSkeletonAnimController(retCon, new GHModelSkeletonAnimCloner(*retCon));
}
