// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHInputState;
class GHControllerMgr;

class GHTestAppRagdollInputXMLLoader : public GHXMLObjLoader
{
public:
    GHTestAppRagdollInputXMLLoader(const GHInputState& inputState, GHControllerMgr& controllerMgr);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHInputState& mInputState;
    GHControllerMgr& mControllerMgr;
};
