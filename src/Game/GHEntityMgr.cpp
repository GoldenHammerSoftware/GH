// Copyright Golden Hammer Software
#include "GHEntityMgr.h"
#include "GHEntity.h"
#include "GHXMLObjFactory.h"
#include "GHEntityHashes.h"
#include "GHString/GHString.h"

GHEntityMgr::GHEntityMgr(const GHXMLObjFactory& objFactory)
: mObjFactory(objFactory)
{
}

GHEntityMgr::~GHEntityMgr(void)
{
    killAllEnts();
}

GHEntity* GHEntityMgr::spawnEnt(const GHString& fileName, int entId)
{
    killEnt(entId);
    if (fileName.isEmpty()) {
        return 0;
    }
    
    GHPropertyContainer extraData;
    extraData.setProperty(GHEntityHashes::P_ENTID, entId);
    
    GHEntity* ent = (GHEntity*)mObjFactory.load(fileName, &extraData);
    if (!ent) return 0;
    
    ent->mProperties.setProperty(GHEntityHashes::P_ENTID, entId);
    GHString* propStr = new GHString(fileName);
    ent->mProperties.setProperty(GHEntityHashes::P_ENTFILE, GHProperty(propStr, new GHRefCountedType<GHString>(propStr)));
    mEnts[entId] = ent;
    ent->mStateMachine.setState(GHEntityHashes::ES_ACTIVE);
    return ent;
}

void GHEntityMgr::killEnt(int entId)
{
    EntList::iterator findIter;
    findIter = mEnts.find(entId);
    if (findIter == mEnts.end()) return;
    delete findIter->second;
    mEnts.erase(findIter);
}

void GHEntityMgr::killAllEnts(void)
{
    EntList::iterator entIter;
    for (entIter = mEnts.begin(); entIter != mEnts.end(); ++entIter)
    {
        delete entIter->second;
    }
    mEnts.clear();
}

GHEntity* GHEntityMgr::findEnt(int entId) const
{
    EntList::const_iterator findIter;
    findIter = mEnts.find(entId);
    if (findIter == mEnts.end()) return 0;
    return findIter->second;
}

