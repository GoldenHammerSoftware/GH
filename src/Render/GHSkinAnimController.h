// Copyright Golden Hammer Software
#pragma once

#include "Sim/GHAnimController.h"

class GHVertexBuffer;
class GHTransformNode;

// interface for an anim controller that targets a vb based on a skeleton.
class GHSkinAnimController : public GHAnimController
{
public:
    virtual void setTarget(GHVertexBuffer* targetVB, GHTransformNode* targetSkeleton) = 0;
    virtual const GHVertexBuffer* getTargetVB(void) const = 0;

    // skin anims do not have anim sequences.
    virtual void setAnim(const GHIdentifier& id, float startTime=0) {}
    virtual void setAnimBlend(const std::vector<AnimBlend>& blends) {}

    virtual AnimType getAnimType(void) const { return AT_SKIN; }
};
