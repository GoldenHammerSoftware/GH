#pragma once

#include "GHMultiplayerFactory.h"
#include "GHTestMultiplayerMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMultiplayerIdentifiers.h"
#include "GHAppShard.h"
#include "GHString/GHString.h"
#include "GHSystemServices.h"

class GHTestMultiplayerFactory : public GHMultiplayerFactory
{
public:

    virtual GHMultiplayerMgr* createMultiplayerMgr(GHAppShard& appShard)
    {
        appShard.mProps.setProperty(GHMultiplayerIdentifiers::GP_MULTIPLAYERACTIVE, true);
        return new GHTestMultiplayerMgr(appShard.mSystemServices.getPlatformServices().getFileOpener(), mAlternateOurTurn, mSaveFilename);
    }

    // If false, it's always our turn
    void setAlternateOurTurn(bool alternate) { mAlternateOurTurn = alternate; }

    // String will be referenced. Make sure it isn't deleted. If null, no save/load of files.
    void setSaveFilename(const char* saveFilename) { mSaveFilename = saveFilename; }

private:
    bool mAlternateOurTurn{ false };
    const char* mSaveFilename{ 0 };
};
