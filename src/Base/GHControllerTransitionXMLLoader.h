// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHXMLObjFactory;
class GHControllerMgr;

/*
 <controllerTransition priority=0>
    // controllers
 </controllerTransition>
*/
class GHControllerTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHControllerTransitionXMLLoader(GHXMLObjFactory& objFactory,
                                    GHControllerMgr& controllerManager,
                                    const GHIdentifierMap<int>& enumMap);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
    GHXMLObjFactory& mObjFactory;
    GHControllerMgr& mControllerManager;
    const GHIdentifierMap<int>& mEnumMap;
};
