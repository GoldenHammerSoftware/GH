// Copyright Golden Hammer Software
#pragma once

#include "GHMultiplayerFactory.h"
#include "GHNullMultiplayerMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMultiplayerIdentifiers.h"
#include "GHAppShard.h"

class GHNullMultiplayerFactory : public GHMultiplayerFactory
{
public:

    virtual GHMultiplayerMgr* createMultiplayerMgr(GHAppShard& appShard)
    {
        appShard.mProps.setProperty(GHMultiplayerIdentifiers::GP_MULTIPLAYERACTIVE, false);
        return new GHNullMultiplayerMgr;
    }
};
