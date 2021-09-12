// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHGameCenterMultiplayerMgr;
class GHGameCenterNetworkDataMgr;

class GHGameCenterMultiplayerMessageHandler : public GHMessageHandler
{
public:
    GHGameCenterMultiplayerMessageHandler(GHEventMgr& gameThreadeventMgr,
                                          GHEventMgr& uiThreadEventMgr,
                                          GHGameCenterMultiplayerMgr& gameThreadMgr,
                                          GHGameCenterNetworkDataMgr& uiThreadMgr);
    ~GHGameCenterMultiplayerMessageHandler(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHEventMgr& mGameThreadEventMgr;
    GHEventMgr& mUIThreadEventMgr;
    GHGameCenterMultiplayerMgr& mGameThreadMgr;
    GHGameCenterNetworkDataMgr& mUIThreadMgr;
};
