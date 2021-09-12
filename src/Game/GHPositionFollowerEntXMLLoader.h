// Copyright Golden Hammer Software
#pragma once

#include "Base/GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHTransform;
class GHControllerMgr;
class GHStringIdFactory;

// creates a GHPositionFollower that changes an entity's position
// apps should create an instance of this with the target transform and a unique load key for the node.
// <loaderkey (priority=0 or priorityEnum=UP_SKYBOX) targetNode=snowboard/>
class GHPositionFollowerEntXMLLoader : public GHXMLObjLoader
{
public:
    GHPositionFollowerEntXMLLoader(GHTransform& other, GHControllerMgr& conMgr,
                                   bool copyToEnt, const GHIdentifierMap<int>& enumMap,
								   GHStringIdFactory& hashTable);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHTransform& mOther;
    GHControllerMgr& mConMgr;
    const GHIdentifierMap<int>& mEnumMap;
	GHStringIdFactory& mIdFactory;
    bool mCopyToEnt;
};
