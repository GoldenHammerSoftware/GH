// Copyright 2010 Golden Hammer Software
#include "GHGameCenterMultiplayerMessageHandler.h"
#include "GHGameCenterMultiplayerMgr.h"
#include "GHGameCenterNetworkDataMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHAppleIdentifiers.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMultiplayerMatchOutcome.h"
#include "GHString/GHString.h"

GHGameCenterMultiplayerMessageHandler::GHGameCenterMultiplayerMessageHandler(GHEventMgr& gameThreadEventMgr,
                                                                             GHEventMgr& uiThreadEventMgr,
                                                                             GHGameCenterMultiplayerMgr& gameThreadMgr,
                                                                             GHGameCenterNetworkDataMgr& uiThreadMgr)
: mGameThreadEventMgr(gameThreadEventMgr)
, mUIThreadEventMgr(uiThreadEventMgr)
, mGameThreadMgr(gameThreadMgr)
, mUIThreadMgr(uiThreadMgr)
{
    mUIThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERMATCHREQUESTED, *this);
    mUIThreadEventMgr.registerListener(GHAppleIdentifiers::NEXTMULTIPLAYERMATCHREQUESTED, *this);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERMATCHRETURNED, *this);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERMATCHCANCELLED, *this);
    mUIThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERTURNTAKEN, *this);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERACTIVEMATCHNUMBERCHANGED, *this);
    mUIThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERCLEARMATCH, *this);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::MULTIPLAYERMATCHDIALOGDISPLAYED, *this);
}

GHGameCenterMultiplayerMessageHandler::~GHGameCenterMultiplayerMessageHandler(void)
{
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERMATCHDIALOGDISPLAYED, *this);
    mUIThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERCLEARMATCH, *this);
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERACTIVEMATCHNUMBERCHANGED, *this);
    mUIThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERTURNTAKEN, *this);
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERMATCHCANCELLED, *this);
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERMATCHRETURNED, *this);
    mUIThreadEventMgr.unregisterListener(GHAppleIdentifiers::NEXTMULTIPLAYERMATCHREQUESTED, *this);
    mUIThreadEventMgr.unregisterListener(GHAppleIdentifiers::MULTIPLAYERMATCHREQUESTED, *this);
}

void GHGameCenterMultiplayerMessageHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHAppleIdentifiers::MULTIPLAYERMATCHREQUESTED)
    {
        mUIThreadMgr.displayMultiplayerView();
    }
    else if (message.getType() == GHAppleIdentifiers::NEXTMULTIPLAYERMATCHREQUESTED)
    {
        mUIThreadMgr.loadNextMatch();
    }
    else if (message.getType() == GHAppleIdentifiers::MULTIPLAYERMATCHRETURNED)
    {
        void* data = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATA);
        int length = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATALENGTH);
        bool isOurTurn = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHISOURTURN);
        std::vector<GHString>* playerNames = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERNAMES);
        mGameThreadMgr.handleMatchReturnedFromGC(isOurTurn, data, length, playerNames);
        
        //this data was allocated by the sender in the UI thread
        delete[] (char*)data;
        delete playerNames;
    }
    else if(message.getType() == GHAppleIdentifiers::MULTIPLAYERMATCHDIALOGDISPLAYED)
    {
        mGameThreadMgr.handleMatchSelectDialogPresented();
    }
    else if(message.getType() == GHAppleIdentifiers::MULTIPLAYERMATCHCANCELLED)
    {
        mGameThreadMgr.handleMatchCancelledByGC();
    }
    else if(message.getType() == GHAppleIdentifiers::MULTIPLAYERTURNTAKEN)
    {
        void* data = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATA);
        int length = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATALENGTH);
        bool gameIsOver = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHGAMEISOVER);
        if (gameIsOver)
        {
            const std::vector<GHMultiplayerMatchOutcome::Enum>* results = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHRESULTS);
            mUIThreadMgr.submitGameEndToGameCenter(data, length, results);
        }
        else 
        {
            mUIThreadMgr.submitTurnToGameCenter(data, length);
        }
        //this data was allocated by the sender in the game thread
        delete[] (char*)data;
    }
    else if (message.getType() == GHAppleIdentifiers::MULTIPLAYERACTIVEMATCHNUMBERCHANGED)
    {
        int count = message.getPayload().getProperty(GHAppleIdentifiers::MULTIPLAYERMATCHCOUNT);
        mGameThreadMgr.handleActiveMatchCountChanged(count);
    }
    else if (message.getType() == GHAppleIdentifiers::MULTIPLAYERCLEARMATCH)
    {
        mUIThreadMgr.clearCurrentMatch();
    }
}
