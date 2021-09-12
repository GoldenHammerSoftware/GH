// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringHashTable;
class GHControllerMgr;

class CGCharacterAnimControllerXMLLoader : public GHXMLObjLoader
{
public:
    CGCharacterAnimControllerXMLLoader(const GHStringHashTable& hashtable,
                                       GHControllerMgr& controllerMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHStringHashTable& mHashtable;
    GHControllerMgr& mControllerMgr;
};
