// Copyright Golden Hammer Software
#include "GHCSGameLauncher.h"
#include "GHCSIdentifiers.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHSystemServices.h"
#include "GHConnectionHandleLocal.h"
#include "GHCSClient.h"
#include "GHCSServer.h"
#include <assert.h>

GHCSGameLauncher::GHCSGameLauncher(GHEventMgr& eventMgr, GHSystemServices& systemServices,
                                   GHCSClient& client, GHCSServer& server)
: mEventMgr(eventMgr)
, mSystemServices(systemServices)
, mClient(client)
, mServer(server)
, mLaunchState(LS_DISCONNECTED)
, mLevelName("")
{
    mEventMgr.registerListener(GHCSIdentifiers::M_STARTLOCAL, *this);
    mEventMgr.registerListener(GHCSIdentifiers::M_DISCONNECT, *this);
    mEventMgr.registerListener(GHCSIdentifiers::M_SETLOCALLEVEL, *this);
}

GHCSGameLauncher::~GHCSGameLauncher(void)
{
    mEventMgr.unregisterListener(GHCSIdentifiers::M_STARTLOCAL, *this);
    mEventMgr.unregisterListener(GHCSIdentifiers::M_DISCONNECT, *this);
    mEventMgr.unregisterListener(GHCSIdentifiers::M_SETLOCALLEVEL, *this);
}

void GHCSGameLauncher::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHCSIdentifiers::M_STARTLOCAL)
    {
        startLocal();
    }
    else if (message.getType() == GHCSIdentifiers::M_DISCONNECT)
    {
        disconnect();
    }
    else if (message.getType() == GHCSIdentifiers::M_SETLOCALLEVEL)
    {
        const GHString* levelName = (const GHString*)message.getPayload().getProperty(GHCSIdentifiers::P_LEVELNAME);
        assert(levelName);
        setLocalLevel(*levelName);
    }
}

void GHCSGameLauncher::startLocal(void)
{
    disconnect();
    mServer.setLevel(mLevelName);
    
    GHConnectionHandleLocal* clientToServerHandle = new GHConnectionHandleLocal(&mSystemServices.getPlatformServices().getThreadFactory());
    GHConnectionHandleLocal* serverToClientHandle = new GHConnectionHandleLocal(&mSystemServices.getPlatformServices().getThreadFactory());
    clientToServerHandle->setTarget(serverToClientHandle);
    serverToClientHandle->setTarget(clientToServerHandle);
    mClient.connect(clientToServerHandle);
    mServer.connect(serverToClientHandle);
    
    mLaunchState = LS_RUNNINGLOCAL;
}

void GHCSGameLauncher::disconnect(void)
{
    if (mLaunchState == LS_DISCONNECTED) {
        // not connected.
        return;
    }
    // todo: handle non-local connections.
    mServer.disconnectAll();
    mServer.setLevel("");
    mClient.disconnect();
    mLaunchState = LS_DISCONNECTED;
}

void GHCSGameLauncher::setLocalLevel(const GHString& levelName)
{
    mLevelName = levelName;
    if (mLaunchState == LS_RUNNINGLOCAL) {
        mServer.setLevel(levelName);
    }
}
