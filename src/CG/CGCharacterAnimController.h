// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHIdentifier.h"

class GHEntity;
class GHModel;

class CGCharacterAnimController : public GHController
{
public:
    CGCharacterAnimController(const GHEntity& entity,
                              GHModel& model,
                              const GHIdentifier& idleAnim,
                              const GHIdentifier& runAnim,
                              const GHIdentifier& selectedAnim,
                              const GHIdentifier& attackAnim,
                              const GHIdentifier& deathAnim);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
 
private:
    const GHEntity& mEntity;
    GHModel& mModel;
    
    GHIdentifier mIdleAnim;
    GHIdentifier mRunAnim;
    GHIdentifier mSelectedAnim;
    GHIdentifier mAttackAnim;
    GHIdentifier mDeathAnim;
};
