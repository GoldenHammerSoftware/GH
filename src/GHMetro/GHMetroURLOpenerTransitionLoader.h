// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

// <openURL url="http://www.goldenhammersoftware.com"/>
class GHMetroURLOpenerTransitionLoader : public GHXMLObjLoader
{
public:
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
};
