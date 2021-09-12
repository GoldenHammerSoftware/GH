// Copyright Golden Hammer Software
#include "GHCSClient.h"
#include "GHAppShard.h"
#include "GHConnectionHandleController.h"
#include "GHConnectionHandle.h"

GHCSClient::GHCSClient(GHAppShard& appShard)
: mAppShard(appShard)
, mEntityMgr(appShard.mXMLObjFactory)
, mEntityMgrListener(mEntityMgr, appShard.mEventMgr)
, mConnection(0)
, mConnectionUpdater(0)
{
    mConnectionUpdater = new GHConnectionHandleController();
    mAppShard.mControllerMgr.add(mConnectionUpdater);
}

GHCSClient::~GHCSClient(void)
{
    mAppShard.mControllerMgr.remove(mConnectionUpdater);
    delete mConnectionUpdater;
}

void GHCSClient::connect(GHConnectionHandle* connection)
{
    disconnect();
    mConnection = connection;
    mConnectionUpdater->setTarget(connection , &mAppShard.mEventMgr);
    onConnect();
}

void GHCSClient::disconnect(void)
{
    onDisconnect();
    mConnectionUpdater->setTarget(0,0);
    delete mConnection;
    mConnection = 0;
}
