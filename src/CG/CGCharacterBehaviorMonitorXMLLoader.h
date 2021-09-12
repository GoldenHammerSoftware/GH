// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHTimeVal;
class GHControllerMgr;

class CGCharacterBehaviorMonitorXMLLoader : public GHXMLObjLoader
{
public:
    CGCharacterBehaviorMonitorXMLLoader(const GHTimeVal& timeVal,
                                        GHControllerMgr& controllerMgr);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
private:
    const GHTimeVal& mTimeVal;
    GHControllerMgr& mControllerMgr;
};
