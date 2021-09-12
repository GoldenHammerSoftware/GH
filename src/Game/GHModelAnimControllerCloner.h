// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHModel;
class GHAnimController;
class GHTransformNode;

// interface for cloning an anim controller based on cloning a model.
// knows what type of info is needed from the model.
class GHModelAnimControllerCloner
{
public:
    virtual ~GHModelAnimControllerCloner(void) {}
    virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes) = 0;
};
