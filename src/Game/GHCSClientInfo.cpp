// Copyright Golden Hammer Software
#include "GHCSClientInfo.h"
#include "GHConnectionHandle.h"
#include <assert.h>

GHCSClientInfo::GHCSClientInfo(GHConnectionHandle* connection, int clientId)
: mClientId(clientId)
, mConnection(connection)
{
    assert(mConnection);
}

GHCSClientInfo::~GHCSClientInfo(void)
{
    delete mConnection;
}

