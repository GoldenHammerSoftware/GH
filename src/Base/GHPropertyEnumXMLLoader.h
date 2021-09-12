// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

// <enumProp val=blah/>
class GHPropertyEnumXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyEnumXMLLoader(const GHIdentifierMap<int>& enumStore);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHIdentifierMap<int>& mEnumStore;
};
