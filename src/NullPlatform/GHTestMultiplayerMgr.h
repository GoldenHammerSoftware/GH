#pragma once

#include "GHMultiplayerMgr.h"
#include "GHMultiplayerMatchCallback.h"

class GHFileOpener;

//Slightly more complex than GHNullMultiplayerMgr: saves the turn and sends it right back to you.
// Intended for testing multiplayer logic on a single machine.
class GHTestMultiplayerMgr : public GHMultiplayerMgr
{
public:
    GHTestMultiplayerMgr(const GHFileOpener& fileOpener, bool alternateOurTurn, const char* saveFilename);

    void startMatch(void);

    virtual void startNextMatch(void);

    virtual void sendTurn(const void* data, int dataLength);

    virtual void sendGameOver(const void* data, int dataLength, const std::vector<GHMultiplayerMatchOutcome::Enum>& results);

    virtual const char* getLocalPlayerAlias(void) const;

    virtual void setGameCallback(GHMultiplayerMatchCallback* callback);

    virtual void clearCurrentMatch(void);

    void loadTurn(void);

private:
    void saveTurn(void) const;
        
private:
    const GHFileOpener& mFileOpener;
    GHMultiplayerMatchCallback* mCallback{ 0 };
    std::vector<uint8_t> mLastSentTurnData;
    std::vector<GHString> mPlayerNames;
    const char* mSaveFilename{ false };
    bool mAlternateOurTurn{ false };
    bool mIsOurTurn{ false };

};
