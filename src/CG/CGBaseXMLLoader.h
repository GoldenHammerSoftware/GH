// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringHashTable;

class CGBaseXMLLoader : public GHXMLObjLoader
{
public:
    CGBaseXMLLoader(const GHXMLObjFactory& objFactory,
                    const GHStringHashTable& hashtable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHXMLObjFactory& mObjFactory;
    const GHStringHashTable& mHashtable;
};
