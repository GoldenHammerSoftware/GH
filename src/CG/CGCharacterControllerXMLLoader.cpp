// Copyright 2010 Golden Hammer Software
#include "CGCharacterControllerXMLLoader.h"
#include "CGCharacterController.h"
#include "GHXMLNode.h"
#include "GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHControllerTransition.h"

CGCharacterControllerXMLLoader::CGCharacterControllerXMLLoader(GHControllerMgr& controllerMgr)
: mControllerMgr(controllerMgr)
{
    
}

void* CGCharacterControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* entity = (GHEntity*)extraData.getProperty(GHEntityHashes::ENTITY);
    if (!entity) { return 0; }
    
    return new GHControllerTransition(mControllerMgr, new CGCharacterController(*entity));
}
