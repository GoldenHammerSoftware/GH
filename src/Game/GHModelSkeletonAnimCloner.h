// Copyright Golden Hammer Software
#pragma once

#include "GHModelAnimControllerCloner.h"

class GHSkeletonAnimController;

class GHModelSkeletonAnimCloner : public GHModelAnimControllerCloner
{
public:
    GHModelSkeletonAnimCloner(GHSkeletonAnimController& con);
    
    virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes);
    
private:
    GHSkeletonAnimController& mCon;
};
