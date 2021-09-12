// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHString.h"

class GHEventMgr;
class GHSystemServices;
class GHCSClient;
class GHCSServer;
class GHConnectionHandle;

// a class to handle the setup of client and server for
//  starting or connecting to a game.
// listens for messages on the gui event mgr
class GHCSGameLauncher : public GHMessageHandler
{
public:
    GHCSGameLauncher(GHEventMgr& eventMgr, GHSystemServices& systemServices,
                     GHCSClient& client, GHCSServer& server);
    ~GHCSGameLauncher(void);
    
    virtual void handleMessage(const GHMessage& message);
    
    void startLocal(void);
    void disconnect(void);
    void setLocalLevel(const GHString& levelName);

private:
    enum LaunchState
    {
        LS_DISCONNECTED,
        LS_RUNNINGLOCAL,
        LS_RUNNINGINTERNET
    };
    
private:
    GHEventMgr& mEventMgr;
    GHSystemServices& mSystemServices;
    GHCSClient& mClient;
    GHCSServer& mServer;
    
    LaunchState mLaunchState;
    GHString mLevelName;
};
