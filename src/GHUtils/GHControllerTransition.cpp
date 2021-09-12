// Copyright Golden Hammer Software
#include "GHControllerTransition.h"
#include "GHControllerMgr.h"
#include "GHController.h"

GHControllerTransition::GHControllerTransition(GHControllerMgr& controllerMgr, GHController* controller,
                                               float priority)
: mControllerMgr(controllerMgr)
, mPriority(priority)
{
    mControllers.push_back(controller);
}

GHControllerTransition::GHControllerTransition(GHControllerMgr& controllerMgr, 
                                               const std::vector<GHController*>& controllers,
                                               float priority)
: mControllerMgr(controllerMgr)
, mPriority(priority)
{
    mControllers = controllers;
}

GHControllerTransition::~GHControllerTransition(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        // if we call remove here, then we might be calling deactivate twice.
        // should only call remove if we are active.
        //mControllerMgr.remove(mControllers[i]);
        delete mControllers[i];
    }
}

void GHControllerTransition::activate(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        mControllerMgr.add(mControllers[i], mPriority);
    }
}

void GHControllerTransition::deactivate(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        mControllerMgr.remove(mControllers[i]);
    }
}

