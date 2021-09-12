// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPoint.h"

class GHInputState;
class GHEntity;

class GHTestAppRagdollInput : public GHController
{
public:
    GHTestAppRagdollInput(const GHInputState& inputState, GHEntity* entity);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    void applyImpulse(const GHPoint3& impulse);
    
private:
    const GHInputState& mInputState;
    GHEntity* mEntity;
};
