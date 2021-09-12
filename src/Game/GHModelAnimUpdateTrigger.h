// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRunnable.h"

class GHModel;

// Callback to run through the animations of a model.
class GHModelAnimUpdateTrigger : public GHRunnable
{
public:
    GHModelAnimUpdateTrigger(GHModel& model);
    
    virtual void run(void);
    
private:
    GHModel& mModel;
};

