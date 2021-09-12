// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHControllerMgr;

class CGCharacterControllerXMLLoader : public GHXMLObjLoader
{
public:
    CGCharacterControllerXMLLoader(GHControllerMgr& controllerMgr);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
private:
    GHControllerMgr& mControllerMgr;
};
