// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;

// <identifierProp val="IDENTIFIERSTRING"/>
class GHPropertyIdentifierXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyIdentifierXMLLoader(GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHStringIdFactory& mIdFactory;
};
