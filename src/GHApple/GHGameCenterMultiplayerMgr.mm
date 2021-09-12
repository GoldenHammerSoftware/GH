// Copyright 2010 Golden Hammer Software
#include "GHGameCenterMultiplayerMgr.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHGameCenterMultiplayerMessageHandler.h"
#include "GHAppleIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHGameCenterNetworkDataMgr.h" //for delete
#include "GHMultiplayerMatchCallback.h"

GHGameCenterMultiplayerMgr::GHGameCenterMultiplayerMgr(GHMessageQueue& uiMessageQueue,
                                                       GHEventMgr& uiThreadEventMgr,
                                                       GHEventMgr& gameThreadEventMgr,
                                                       GHGameCenterNetworkDataMgr* networkDataMgr)
: mUIMessageQueue(uiMessageQueue)
, mNetworkDataMgr(networkDataMgr)
, mGameCallback(0)
, mActiveMatchCountToReport(0)
{
    mMessageHandler = new GHGameCenterMultiplayerMessageHandler(gameThreadEventMgr, uiThreadEventMgr, *this, *mNetworkDataMgr);
}

GHGameCenterMultiplayerMgr::~GHGameCenterMultiplayerMgr(void)
{
    delete mMessageHandler;
    delete mNetworkDataMgr;
}

void GHGameCenterMultiplayerMgr::startMatch(void)
{
    GHPropertyContainer payload;
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERMATCHREQUESTED, &payload);
    mUIMessageQueue.handleMessage(message);
}

void GHGameCenterMultiplayerMgr::startNextMatch(void)
{
    GHPropertyContainer payload;
    GHMessage message(GHAppleIdentifiers::NEXTMULTIPLAYERMATCHREQUESTED);
    mUIMessageQueue.handleMessage(message);
}

void GHGameCenterMultiplayerMgr::sendTurn(const void* data, int length)
{
    GHPropertyContainer payload;
    sendTurn(data, length, payload);
}

void GHGameCenterMultiplayerMgr::sendGameOver(const void* data, int length, const std::vector<GHMultiplayerMatchOutcome::Enum>& results)
{
    GHPropertyContainer payload;
    payload.setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHGAMEISOVER, true);
    std::vector<GHMultiplayerMatchOutcome::Enum>* newResults = new std::vector<GHMultiplayerMatchOutcome::Enum>(results);
    payload.setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHRESULTS, 
                        GHProperty(newResults, new GHRefCountedType<std::vector<GHMultiplayerMatchOutcome::Enum> >(newResults)));
    sendTurn(data, length, payload);
}

void GHGameCenterMultiplayerMgr::sendTurn(const void* data, int length, GHPropertyContainer& payload)
{
    //to be deleted in recipient thread
    void* copiedData = 0;
    if (data)
    {
        copiedData = new char[length];
        memcpy(copiedData, data, length);
    }
    
    payload.setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATA, copiedData);
    payload.setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATALENGTH, length);
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERTURNTAKEN, &payload);
    mUIMessageQueue.handleMessage(message);
}

void GHGameCenterMultiplayerMgr::clearCurrentMatch(void)
{
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERCLEARMATCH, 0);
    mUIMessageQueue.handleMessage(message);
}

const char* GHGameCenterMultiplayerMgr::getLocalPlayerAlias(void) const
{
    return mNetworkDataMgr->getLocalPlayerAlias();
}

void GHGameCenterMultiplayerMgr::handleMatchSelectDialogPresented(void)
{
    if(mGameCallback) { mGameCallback->matchSelectDialogDisplayed(); }
}

void GHGameCenterMultiplayerMgr::handleMatchReturnedFromGC(bool isOurTurn, void* data, int length, const std::vector<GHString>* playerNames)
{
    if (mGameCallback) { mGameCallback->matchReturned(isOurTurn, data, length, playerNames); }
}

void GHGameCenterMultiplayerMgr::handleMatchCancelledByGC(void)
{
    if (mGameCallback) { mGameCallback->matchCancelled(); }
}

void GHGameCenterMultiplayerMgr::setGameCallback(GHMultiplayerMatchCallback* callback)
{
    if (!mGameCallback && mActiveMatchCountToReport)
    {
        callback->activeMatchCountChanged(mActiveMatchCountToReport);
        mActiveMatchCountToReport = 0;
    }
    mGameCallback = callback;
}

void GHGameCenterMultiplayerMgr::handleActiveMatchCountChanged(int activeMatchCount)
{
    if (mGameCallback)
    {
        mGameCallback->activeMatchCountChanged(activeMatchCount);
    }
    else 
    {
        mActiveMatchCountToReport = activeMatchCount;
    }
}
