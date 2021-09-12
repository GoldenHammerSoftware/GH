// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

// <stringProp val="blah"/>
// creates a GHString* property with a ref count.
class GHPropertyStringXMLLoader : public GHXMLObjLoader
{
public:
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
};
