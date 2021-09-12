// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHTimeVal;
class GHEntity;
class GHModel;

class CGCharacterBehaviorMonitor : public GHController
{
public:
    CGCharacterBehaviorMonitor(const GHTimeVal& time,
                               GHEntity& entity,
                               GHModel& model);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    void checkAttack(void);
    void checkReachedDestination(void);
    
private:
    const GHTimeVal& mTime;
    GHEntity& mEntity;
    GHModel& mModel;
    
    float mAttackAnimTime;
    float mTimeSpentAttacking;
};
