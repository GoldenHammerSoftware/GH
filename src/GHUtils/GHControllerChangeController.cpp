// Copyright Golden Hammer Software
#include "GHControllerChangeController.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHPlatform/GHMutex.h"
#include <stddef.h> //for size_t

GHControllerChangeController::GHControllerChangeController(GHThreadFactory& threadFactory,
                                                           GHControllerMgr& controllerMgr)
: mControllerMgr(controllerMgr)
{
    mMutex = threadFactory.createMutex();
}

GHControllerChangeController::~GHControllerChangeController(void)
{
    delete mMutex;
}

void GHControllerChangeController::update(void)
{
    if (mMutex) mMutex->acquire();
    size_t numChanges = mControllerChanges.size();
    for (size_t i = 0; i < numChanges; ++i)
    {
        ControllerChange& change = mControllerChanges[i];
        if (change.mIsAdd)
        {
            mControllerMgr.add(change.mController, change.mPriority);
        }
        else 
        {
            mControllerMgr.remove(change.mController);
        }
    }
    mControllerChanges.resize(0);
    if (mMutex) mMutex->release();
}

void GHControllerChangeController::onActivate(void)
{
    
}

void GHControllerChangeController::onDeactivate(void)
{
    
}

void GHControllerChangeController::add(GHController* controller, float priority)
{
    if (mMutex) mMutex->acquire();
    mControllerChanges.push_back(ControllerChange(controller, priority, true));
    if (mMutex) mMutex->release();
}

void GHControllerChangeController::remove(GHController* controller)
{
    if (mMutex) mMutex->acquire();
    mControllerChanges.push_back(ControllerChange(controller, 0, false));
    if (mMutex) mMutex->release();
}
