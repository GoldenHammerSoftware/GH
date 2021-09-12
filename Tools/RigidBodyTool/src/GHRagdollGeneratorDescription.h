// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHString.h"
#include "GHIdentifier.h"
#include <vector>

class GHBulletPhysicsObject;

struct GHRagdollGeneratorDescription
{
    struct NodeDesc
    {
        GHString mParentName;
        GHString mTargetName;
        GHString mPivotName;
        GHString mCenterName;
        GHIdentifier mParentID;
        GHIdentifier mTargetID;
        GHIdentifier mPivotID;
        GHIdentifier mCenterID;
        float mRadius;
    };
    std::vector<NodeDesc> mNodes;
};
