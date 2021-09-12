// Copyright Golden Hammer Software
#include "GHConnectionHandleController.h"
#include "GHConnectionHandle.h"
#include "GHUtils/GHMessageHandler.h"

GHConnectionHandleController::GHConnectionHandleController(void)
: mConnection(0)
, mHandler(0)
{
}

void GHConnectionHandleController::setTarget(GHConnectionHandle* connection, GHMessageHandler* handler)
{
    mConnection = connection;
    mHandler = handler;
}

void GHConnectionHandleController::update(void)
{
    if (!mHandler || !mConnection) {
        return;
    }
    mConnection->readMessages(*mHandler);
}

