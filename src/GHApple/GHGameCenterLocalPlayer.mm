// Copyright 2010 Golden Hammer Software
#include "GHGameCenterLocalPlayer.h"
#import <GameKit/GameKit.h>
#include "GHGameCenterLog.h"
#include "GHGameCenter.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGameCenterInterruptHandler.h"
#include "GHMessageTypes.h"
#include "GHGameCenterNetworkDataMgr.h"
#include "GHAppleIdentifiers.h"

GHGameCenterLocalPlayer::GHGameCenterLocalPlayer(GHPropertyContainer& propertyContainer)
: mPropertyContainer(propertyContainer)
, mLocalPlayer(0)
, mAchievementListToNotify(0)
, mMultiplayerMgrToNotify(0)
, mAuthenticationIsInProgress(false)
, mEnabled(true)
{

}

GHGameCenterLocalPlayer::~GHGameCenterLocalPlayer(void)
{

}

void GHGameCenterLocalPlayer::handleAuthenticationFinished(bool authenticated)
{
    GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
    if (localPlayer.isAuthenticated)
    {
        mLocalPlayer = localPlayer;
        setActive(true);
    }
    else
    {
        mLocalPlayer = 0;
        setActive(false);
    }

    if (mAchievementListToNotify)
    {
        mAchievementListToNotify->handleAuthenticationHappened(authenticated);
    }
    if (mMultiplayerMgrToNotify)
    {
        mMultiplayerMgrToNotify->handleAuthenticationHappened(authenticated);
    }
    mAuthenticationIsInProgress = false;
}

void GHGameCenterLocalPlayer::authenticateOldStyle(void)
{
    // deprecated way (ios5)
    GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
    [localPlayer authenticateWithCompletionHandler:^(NSError *error)
     {
         GHDebugMessage::outputString("Local Player Completion Handler called.");
        
         if (error != nil)
         {
             switch ([error code])
             {
                 case GKErrorNotSupported:
                 {
                     disable();
                     break;
                 }
             }
             GHGameCenterLog::printError(error);
             handleAuthenticationFinished(false);
         }
         else
         {
             handleAuthenticationFinished(true);
         }
     }
     ];
}

void GHGameCenterLocalPlayer::authenticate(GHGameCenterNetworkDataMgr* multiplayerMgr)
{
    if (!isEnabled())
    {
        multiplayerMgr->handleAuthenticationHappened(false);
        return;
    }
    mMultiplayerMgrToNotify = multiplayerMgr;
    
    if (isAuthenticated())
    {
        multiplayerMgr->handleAuthenticationHappened(true);
        return;
    }
    else 
    {
        authenticate();
    }
}

void GHGameCenterLocalPlayer::authenticate(GHGameCenter* achievementList)
{
    if (!isEnabled())
    {
        achievementList->handleAuthenticationHappened(false);
        return;
    }
    
    mAchievementListToNotify = achievementList;
    
    if (isAuthenticated())
    {
        achievementList->handleAuthenticationHappened(true);
        return;
    }
    else 
    {
        authenticate();
    }
}

bool GHGameCenterLocalPlayer::isAuthenticated(void) const
{
    return mLocalPlayer != 0;
}

void GHGameCenterLocalPlayer::addActivePropIndex(const GHIdentifier& activePropIndex)
{
    mActivePropIndices.push_back(activePropIndex);
}

bool GHGameCenterLocalPlayer::isEnabled(void) const
{
    return mEnabled;
}

void GHGameCenterLocalPlayer::setActive(bool active)
{
    size_t numProps = mActivePropIndices.size();
    for (size_t i = 0; i < numProps; ++i)
    {
        const GHIdentifier& propIndex = mActivePropIndices[i];
        mPropertyContainer.setProperty(propIndex, active);
    }
}

void GHGameCenterLocalPlayer::disable(void)
{
    mEnabled = false;

    size_t numProps = mActivePropIndices.size();
    for (size_t i = 0; i < numProps; ++i)
    {
        const GHIdentifier& propIndex = mActivePropIndices[i];
        mPropertyContainer.setProperty(propIndex, false);
    }
}

const char* GHGameCenterLocalPlayer::getAlias(void) const
{
    return [[mLocalPlayer alias] cStringUsingEncoding:[NSString defaultCStringEncoding]];
}

void GHGameCenterLocalPlayer::handleAuthenticationViewController(UIViewController* viewcontroller)
{
    mAuthenticateView = viewcontroller;
    bool hasAuth = (mAuthenticateView != 0);
    mPropertyContainer.setProperty(GHAppleIdentifiers::GP_GC_AUTHVIEWWAITING, hasAuth);
}

void GHGameCenterLocalPlayer::registerLocalPlayerListener(NSObject<GKLocalPlayerListener>* listener)
{
    [mLocalPlayer registerListener:listener];
}

