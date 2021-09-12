// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHTransform.h"
#include "GHRagdollGenerator.h"

class GHTransition;
class GHEntity;
class GHModel;

class GHRagdollTool : public GHController
{
public:
    GHRagdollTool(GHTransition* transition,
                  GHEntity& entity,
                  GHRagdollGenerator* generator,
                  const GHTransform& spawnTrans);
    virtual ~GHRagdollTool(void);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    GHRagdollGenerator* mGenerator;
    GHTransition* mTransition;
    GHEntity& mEntity;
    GHModel& mModel;
    GHTransform mSpawnTrans;
};
