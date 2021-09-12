// Copyright Golden Hammer Software
#include "GHEntityMgrMessageListener.h"
#include "GHUtils/GHEventMgr.h"
#include "GHEntityMgr.h"
#include "GHEntityHashes.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"

GHEntityMgrMessageListener::GHEntityMgrMessageListener(GHEntityMgr& entMgr, GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
, mEntityMgr(entMgr)
{
    mEventMgr.registerListener(GHEntityHashes::M_LOADENTITY, *this);
    mEventMgr.registerListener(GHEntityHashes::M_KILLENTITY, *this);
}

GHEntityMgrMessageListener::~GHEntityMgrMessageListener(void)
{
    mEventMgr.unregisterListener(GHEntityHashes::M_LOADENTITY, *this);
    mEventMgr.unregisterListener(GHEntityHashes::M_KILLENTITY, *this);
}

void GHEntityMgrMessageListener::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHEntityHashes::M_LOADENTITY)
    {
        int entId = (int)message.getPayload().getProperty(GHEntityHashes::P_ENTID);
        GHString* file = (GHString*)message.getPayload().getProperty(GHEntityHashes::P_ENTFILE);
        if (!file) {
            GHDebugMessage::outputString("M_LOADENTITY with no P_ENTFILE");
            return;
        }
        mEntityMgr.spawnEnt(*file, entId);
    }
    else if (message.getType() == GHEntityHashes::M_KILLENTITY)
    {
        int entId = (int)message.getPayload().getProperty(GHEntityHashes::P_ENTID);
        mEntityMgr.killEnt(entId);
    }
}
