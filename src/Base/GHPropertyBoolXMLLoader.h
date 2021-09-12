// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <assert.h>

// <floatProp val=10/>
class GHPropertyBoolXMLLoader : public GHXMLObjLoader
{
public:
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
};
