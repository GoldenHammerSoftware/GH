#include "GHTestMultiplayerMgr.h"
#include "GHString/GHString.h"
#include "GHPlatform/GHFileOpener.h"

GHTestMultiplayerMgr::GHTestMultiplayerMgr(const GHFileOpener& fileOpener, bool alternateOurTurn, const char* saveFilename)
    : mFileOpener(fileOpener)
    , mAlternateOurTurn(alternateOurTurn)
    , mSaveFilename(saveFilename)
{
    mPlayerNames.reserve(2);
    mPlayerNames.push_back(GHString("TestP1", GHString::CHT_REFERENCE));
    mPlayerNames.push_back(GHString("TestP2", GHString::CHT_REFERENCE));
}

void GHTestMultiplayerMgr::startMatch(void)
{
    loadTurn();

    if (!mLastSentTurnData.size()) {
        mCallback->matchCancelled();
    }

    if (mAlternateOurTurn)
    {
        mIsOurTurn = !mIsOurTurn;
    }
    else
    {
        mIsOurTurn = true; //it's always our turn
    }
    mCallback->matchReturned(mIsOurTurn, mLastSentTurnData.data(), (int)mLastSentTurnData.size(), &mPlayerNames);
}

void GHTestMultiplayerMgr::startNextMatch(void)
{
    startMatch();
}

void GHTestMultiplayerMgr::sendTurn(const void* data, int dataLength)
{
    mLastSentTurnData.resize(dataLength);
    memcpy(mLastSentTurnData.data(), data, dataLength);

    saveTurn();

    mCallback->activeMatchCountChanged(1);
}

void GHTestMultiplayerMgr::sendGameOver(const void* data, int dataLength, const std::vector<GHMultiplayerMatchOutcome::Enum>& results)
{
    //sendTurn(data, dataLength);
    mLastSentTurnData.resize(0);
    mCallback->activeMatchCountChanged(0);
}

const char* GHTestMultiplayerMgr::getLocalPlayerAlias(void) const
{
    return "TestPlayer";
}

void GHTestMultiplayerMgr::setGameCallback(GHMultiplayerMatchCallback* callback)
{
    mCallback = callback;
}

void GHTestMultiplayerMgr::clearCurrentMatch(void)
{

}

void GHTestMultiplayerMgr::saveTurn(void) const
{
    if (!mSaveFilename) {
        return;
    }

    if (!mLastSentTurnData.size()) {
        return;
    }

    GHFile* file = mFileOpener.openFile(mSaveFilename, GHFile::FT_BINARY, GHFile::FM_WRITEONLY);
    if (!file) {
        return;
    }

    file->writeBuffer(mLastSentTurnData.data(), mLastSentTurnData.size());

    file->closeFile();
    delete file;
}

void GHTestMultiplayerMgr::loadTurn(void)
{
    if (!mSaveFilename) {
        return;
    }

    GHFile* file = mFileOpener.openFile(mSaveFilename, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (!file || !file->readIntoBuffer()) {
        return;
    }

    char* buffer = 0;
    size_t bufferSize = 0;
    if (file->getFileBuffer(buffer, bufferSize) && buffer && bufferSize) 
    {
        mLastSentTurnData.resize(bufferSize);
        memcpy(mLastSentTurnData.data(), buffer, bufferSize);

        if (mCallback) { mCallback->activeMatchCountChanged(1); }
    }

    file->closeFile();
    delete file;
}