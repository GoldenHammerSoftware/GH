// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>

class GHControllerMgr;
class GHMutex;
class GHThreadFactory;

//Allows delay of add/remove of controllers on update.
// This class could be used for other purposes, but the need
// driving its existence is OS thread controllers having their
// activate/deactivate methods called from the OS thread when they
// are added/removed (to this class as an intermediary) from the game thread.
class GHControllerChangeController : public GHController
{
public:
    GHControllerChangeController(GHThreadFactory& threadFactory,
                                 GHControllerMgr& controllerMgr);
    ~GHControllerChangeController(void);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
    void add(GHController* controller, float priority=0);
    void remove(GHController* controller);
    
private:
    GHControllerMgr& mControllerMgr;
    GHMutex* mMutex;
    struct ControllerChange
    {
        GHController* mController;
        float mPriority;
        bool mIsAdd;
        ControllerChange(GHController* controller, float priority, bool isAdd)
        : mController(controller), mPriority(priority), mIsAdd(isAdd)
        { }
        ControllerChange(void)
        : mController(0), mPriority(0), mIsAdd(false)
        { }
    };
    typedef std::vector<ControllerChange> ControllerChangeList;
    ControllerChangeList mControllerChanges;
};
