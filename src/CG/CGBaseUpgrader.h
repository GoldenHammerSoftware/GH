// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"
#include <vector>

class GHXMLObjFactory;
class GHEventMgr;
class GHEntity;

class CGBaseUpgrader : public GHMessageHandler
{
public:
    CGBaseUpgrader(const GHXMLObjFactory& objFactory,
                   GHEventMgr& eventMgr,
                   const int& currentPlayer,
                   std::vector<GHEntity*>& entityList);
    
    ~CGBaseUpgrader(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    void upgradeBase(GHEntity& ent);
    
private:
    const GHXMLObjFactory& mObjFactory;
    GHEventMgr& mEventMgr;
    const int& mCurrentPlayer;
    std::vector<GHEntity*>& mEntityList;
};
