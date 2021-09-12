// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHPropertyContainer;
class GHStringIdFactory;

// <properties>
//  <prop name=model><modelProp file=kya.ghm/></prop>
//  <prop name=health><floatProp val=10/></prop>
// </properties>

class GHPropertyContainerXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyContainerXMLLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
};
