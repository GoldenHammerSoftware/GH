// Copyright Golden Hammer Software
#pragma once

#include "GHModelAnimControllerCloner.h"

class GHTransformAnimController;

// clones GHTransformAnimController for a model
class GHModelTransformAnimCloner : public GHModelAnimControllerCloner
{
public:
    GHModelTransformAnimCloner(GHTransformAnimController& con);
    
    virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes);
    
private:
    GHTransformAnimController& mCon;
};
