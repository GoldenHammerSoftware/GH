// Copyright 2010 Golden Hammer Software
#include "CGBaseUpgrader.h"
#include "CGProperties.h"
#include "GHEventMgr.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "GHEntityHashes.h"
#include "CGPlayer.h"
#include "GHXMLObjFactory.h"
#include "CGBase.h"

CGBaseUpgrader::CGBaseUpgrader(const GHXMLObjFactory& objFactory,
                               GHEventMgr& eventMgr,
                               const int& currentPlayer,
                               std::vector<GHEntity*>& entityList)
: mObjFactory(objFactory)
, mEventMgr(eventMgr)
, mCurrentPlayer(currentPlayer)
, mEntityList(entityList)
{
    mEventMgr.registerListener(CGProperties::UPGRADEBASE, *this);
}

CGBaseUpgrader::~CGBaseUpgrader(void)
{
    mEventMgr.unregisterListener(CGProperties::UPGRADEBASE, *this);
}

void CGBaseUpgrader::handleMessage(const GHMessage& message)
{
    std::vector<GHEntity*>::iterator iter = mEntityList.begin();
    for ( ; iter != mEntityList.end(); ++iter)
    {
        GHEntity* ent = *iter;
        if (!ent) { continue; }
        if ((int)ent->mProperties.getProperty(CGProperties::PP_TEAM) == (mCurrentPlayer+1))
        {
            CGBase* base = ent->mProperties.getProperty(CGProperties::PP_BASE);
            if (base)
            {
                base->addUpgrade("crystalent.xml");
                return;
            }
        }
    }
}
