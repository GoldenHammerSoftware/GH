// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

// <intProp val=10/>
class GHPropertyIntXMLLoader : public GHXMLObjLoader
{
public:
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
};
