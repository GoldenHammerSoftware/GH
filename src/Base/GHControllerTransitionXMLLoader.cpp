// Copyright Golden Hammer Software
#include "GHControllerTransitionXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHXMLNode.h"
#include <assert.h>

GHControllerTransitionXMLLoader::GHControllerTransitionXMLLoader(GHXMLObjFactory& objFactory,
                                                                 GHControllerMgr& controllerManager,
                                                                 const GHIdentifierMap<int>& enumMap)
: mObjFactory(objFactory)
, mControllerManager(controllerManager)
, mEnumMap(enumMap)
{
}

void* GHControllerTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    float priority = 0;
    node.readAttrFloat("priority", priority);
    int priorityEnum = 0;
    if (node.readAttrEnum("priorityEnum", priorityEnum, mEnumMap)) {
        priority = (float)priorityEnum;
    }
    
    std::vector<GHController*> controllers;
    for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        GHController* controller = (GHController*)mObjFactory.load(*node.getChildren()[i], &extraData);
        if (controller) {
            controllers.push_back(controller);
        }
    }
    GHControllerTransition* ret = new GHControllerTransition(mControllerManager, controllers, priority);
    return ret;
}
