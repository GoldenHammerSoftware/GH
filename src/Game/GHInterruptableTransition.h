// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHUtils/GHMessageHandler.h"
#include <vector>

class GHEventMgr;

//Activates/deactivates transitions based on both its own activation state
// and the pause interrupt status. So its child transitions are active while
// it is active and the game is not interrupt-paused. However, if either it is inactive
// or the game is interrupt-paused, the child transitions are considered inactive.
class GHInterruptableTransition : public GHTransition
{
public:
    GHInterruptableTransition(GHEventMgr& eventMgr, GHTransition* trans);
    ~GHInterruptableTransition(void);
    
    virtual void activate(void);
    virtual void deactivate(void);
    
    void handlePauseInterrupt(void);
    void handleUnpauseInterrupt(void);
    
private:
    void activateTransitions(void);
    void deactivateTransitions(void);
    
private:
    class MessageHandler : public GHMessageHandler
    {
    public:
        MessageHandler(GHInterruptableTransition& parent);
        
        virtual void handleMessage(const GHMessage& message);
    private:
        GHInterruptableTransition& mParent;
    };
    
private:
    MessageHandler mMessageHandler;
    GHEventMgr& mEventMgr;
    std::vector<GHTransition*> mTransitions;
    
    bool mIsInterruptPaused;
    bool mIsActive;
};
