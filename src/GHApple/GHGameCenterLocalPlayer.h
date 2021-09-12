// Copyright 2010 Golden Hammer Software
#pragma once

#import <GameKit/GameKit.h>
#include "GHString/GHIdentifier.h"
#include <vector>

class GHPropertyContainer;
class GHGameCenter;
class GHGameCenterInterruptHandler;
class GHGameCenterNetworkDataMgr;

//A class representing the local player on the device.
// Authenticate needs to be called every time the game starts up,
// including coming back from a save (this is handled internally to this class).

//Important: Authenticate must be called from the OS thread. 
class GHGameCenterLocalPlayer
{
public:
    GHGameCenterLocalPlayer(GHPropertyContainer& propertyContainer);
    virtual ~GHGameCenterLocalPlayer(void);
    
    //Must be called from OS thread. Alternate versions set implementation-explicit
    // callbacks that need to happen when the authentication completes.
    virtual void authenticate(void) = 0;
    void authenticate(GHGameCenter* achievementList);
    void authenticate(GHGameCenterNetworkDataMgr* multiplayerMgr);
    
    bool isAuthenticated(void) const;
    
    const char* getAlias(void) const;
    
    void addActivePropIndex(const GHIdentifier& activePropIndex);

    // show the authentication popup if we have it.
    virtual void showAuthenticationViewController(void) {}
    
    //Must be called exactly once after authentication has happened
    // (currently just used by multiplayer mgr). Possible refactor in order here.
    void registerLocalPlayerListener(NSObject<GKLocalPlayerListener>* listener);

protected:
    bool isEnabled(void) const;
    void disable(void);
    void setActive(bool active);
    // callback for when an authentication finishes
    void handleAuthenticationFinished(bool authenticated);
    // use the old deprecated authenticate style.
    void authenticateOldStyle(void);

    // callback for we were given an authentication popup that we can show if we want.
    void handleAuthenticationViewController(UIViewController* viewcontroller);
    
protected:
    GHPropertyContainer& mPropertyContainer;
    bool mEnabled;
    std::vector<GHIdentifier> mActivePropIndices;
    GHGameCenter* mAchievementListToNotify;
    GHGameCenterNetworkDataMgr* mMultiplayerMgrToNotify;
    bool mAuthenticationIsInProgress;
    
    GKLocalPlayer* mLocalPlayer{0};
    UIViewController* mAuthenticateView{0};
};
