// Copyright Golden Hammer Software
#pragma once

#include "GHModelAnimControllerCloner.h"

class GHSkinAnimController;

// clones skin anim controllers attached to models.
class GHModelSkinAnimCloner : public GHModelAnimControllerCloner
{
public:
    GHModelSkinAnimCloner(GHSkinAnimController& con);
    
    virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes);
    
private:
    GHSkinAnimController& mCon;
};
