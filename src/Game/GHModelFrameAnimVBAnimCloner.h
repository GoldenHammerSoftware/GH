// Copyright Golden Hammer Software
#pragma once

#include "GHModelAnimControllerCloner.h"

class GHFrameAnimControllerVB;

class GHModelFrameAnimVBAnimCloner : public GHModelAnimControllerCloner
{
public:
    GHModelFrameAnimVBAnimCloner(GHFrameAnimControllerVB& con);
    
    virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes);
    
private:
    GHFrameAnimControllerVB& mCon;
};
