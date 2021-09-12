// Copyright Golden Hammer Software
#include "GHCSServer.h"
#include "GHAppShard.h"

GHCSServer::GHCSServer(GHAppShard& appShard)
: mEntityMgr(appShard.mXMLObjFactory)
{
}

GHCSServer::~GHCSServer(void)
{
}

void GHCSServer::connect(GHConnectionHandle* connection)
{
    GHCSClientInfo* client = mClients.connect(connection);
    assert(client);
    onConnected(*client);
}

void GHCSServer::disconnect(GHConnectionHandle& connection)
{
    GHCSClientInfo* ci = mClients.getClient(connection);
    if (!ci) {
        return;
    }
    onDisconnected(*ci);
    mClients.disconnect(connection);
}

void GHCSServer::disconnectAll(void)
{
    const std::vector<GHCSClientInfo*>& clients = mClients.getClients();
    for (size_t i = 0; i < clients.size(); ++i)
    {
        onDisconnected(*clients[i]);
    }
    mClients.disconnectAll();
}
