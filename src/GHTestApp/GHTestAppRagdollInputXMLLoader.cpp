// Copyright 2010 Golden Hammer Software
#include "GHTestAppRagdollInputXMLLoader.h"
#include "GHTestAppRagdollInput.h"
#include "GHControllerTransition.h"
#include "GHPropertyContainer.h"
#include "GHEntityHashes.h"

GHTestAppRagdollInputXMLLoader::GHTestAppRagdollInputXMLLoader(const GHInputState& inputState, GHControllerMgr& controllerMgr)
: mInputState(inputState)
, mControllerMgr(controllerMgr)
{
    
}

void* GHTestAppRagdollInputXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::ENTITY);
    return new GHControllerTransition(mControllerMgr, new GHTestAppRagdollInput(mInputState, parentEnt));
}
