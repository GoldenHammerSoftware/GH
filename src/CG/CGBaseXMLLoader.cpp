// Copyright 2010 Golden Hammer Software
#include "CGBaseXMLLoader.h"
#include "CGBase.h"
#include "GHXMLObjFactory.h"
#include "GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHXMLNode.h"
#include "GHStringHashTable.h"

CGBaseXMLLoader::CGBaseXMLLoader(const GHXMLObjFactory& objFactory,
                                 const GHStringHashTable& hashtable)
: mObjFactory(objFactory)
, mHashtable(hashtable)
{
    
}

void* CGBaseXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* baseEnt = (GHEntity*)(extraData.getProperty(GHEntityHashes::ENTITY));
    if (!baseEnt) { return 0; }
    GHIdentifier activeStateId = mHashtable.generateHash("ES_ACTIVE");
    return new CGBase(mObjFactory, *baseEnt, activeStateId);
}
