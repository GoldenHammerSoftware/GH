// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

// <floatProp val=10/>
class GHPropertyFloatXMLLoader : public GHXMLObjLoader
{
public:
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
};
