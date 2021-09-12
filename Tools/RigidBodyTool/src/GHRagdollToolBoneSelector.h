// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPhysicsCursorPicker.h"

class GHInputState;
class GHRagdollToolNode;
class GHPhysicsSim;
class GHRagdollGenerator;
class GHViewInfo;
class GHPropertyContainer;

class GHRagdollToolBoneSelector : public GHController
{
public:
    GHRagdollToolBoneSelector(const GHInputState& inputState,
                              const GHViewInfo& viewInfo,
                              GHPhysicsSim& physicsSim,
                              GHPropertyContainer& properties,
                              GHRagdollGenerator& ragdollGenerator,
                              GHRagdollToolNode*& currentSelection,
                              GHPhysicsObject*& currentPhysicsObject);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);

private:
    GHPhysicsCursorPicker mPicker;
    const GHInputState& mInputState;
    GHPropertyContainer& mProperties;
    GHRagdollGenerator& mRagdollGenerator;
    GHRagdollToolNode*& mCurrentSelection;
    GHPhysicsObject*& mCurrentPhysicsObject;
};
