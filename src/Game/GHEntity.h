// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHStateMachine.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include <vector>

class GHEntity
{
public:
    GHEntity(void);
    ~GHEntity(void);

    void addExistenceTransition(GHTransition* trans);
    
public:
    GHPropertyContainer mProperties;
    GHStateMachine mStateMachine;
    GHEventMgr mEventMgr;
    
private:
    // transitions that are active while this entity exists.
    std::vector<GHTransition*> mExistenceTransitions;
};
