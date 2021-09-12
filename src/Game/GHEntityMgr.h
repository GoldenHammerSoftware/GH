// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <map>

class GHEntity;
class GHString;
class GHXMLObjFactory;

// a class responsible for spawning and keeping track of
//  entities with ent ids.
class GHEntityMgr
{
public:
    typedef std::map<int, GHEntity*> EntList;
    
public:
    GHEntityMgr(const GHXMLObjFactory& objFactory);
    ~GHEntityMgr(void);

    // spawn an ent from filename and assing a guid
    // if entId=0 we calc a new guid.
    GHEntity* spawnEnt(const GHString& fileName, int entId=0);
    // delete an ent with id.
    void killEnt(int entId);
    void killAllEnts(void);
    // find the ent with a given id.
    GHEntity* findEnt(int entId) const;
    
    const EntList& getEnts(void) const { return mEnts; }
    
private:
    const GHXMLObjFactory& mObjFactory;
    EntList mEnts;
};
