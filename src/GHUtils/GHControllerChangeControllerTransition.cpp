// Copyright Golden Hammer Software
#include "GHControllerChangeControllerTransition.h"
#include "GHUtils/GHController.h"
#include "GHControllerChangeController.h"
#include <stddef.h>

GHControllerChangeControllerTransition::GHControllerChangeControllerTransition(GHControllerChangeController& controllerChanger, 
                                                                               GHController* controller,
                                                                               float priority)
: mControllerChanger(controllerChanger)
, mPriority(priority)
{
    mControllers.push_back(controller);
}

GHControllerChangeControllerTransition::GHControllerChangeControllerTransition(GHControllerChangeController& controllerChanger, 
                                                                               const std::vector<GHController*>& controllers,
                                                                               float priority)
: mControllerChanger(controllerChanger)
, mPriority(priority)
{
    mControllers = controllers;
}

GHControllerChangeControllerTransition::~GHControllerChangeControllerTransition(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        delete mControllers[i];
    }
}

void GHControllerChangeControllerTransition::activate(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        mControllerChanger.add(mControllers[i], mPriority);
    }
}

void GHControllerChangeControllerTransition::deactivate(void)
{
    for (size_t i = 0; i < mControllers.size(); ++i)
    {
        mControllerChanger.remove(mControllers[i]);
    }
}
