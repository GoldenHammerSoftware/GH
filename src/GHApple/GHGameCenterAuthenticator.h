// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHEventMgr;
class GHGameCenterLocalPlayer;
class GHGameCenterInterruptHandler;

class GHGameCenterAuthenticator : public GHController
{
public:
    GHGameCenterAuthenticator(GHEventMgr& eventMgr,
                              GHGameCenterLocalPlayer& localPlayer);
    
    ~GHGameCenterAuthenticator(void);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
    void handleUnpause(void);
    bool isAuthenticated(void) const;
    void showAuthView(void);
    
private:
    GHEventMgr& mEventMgr;
    GHGameCenterLocalPlayer& mLocalPlayer;
    GHGameCenterInterruptHandler* mInterruptHandler;
    bool mNeedsAuthentication;
};
