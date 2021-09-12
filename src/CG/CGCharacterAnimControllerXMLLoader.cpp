// Copyright 2010 Golden Hammer Software
#include "CGCharacterAnimControllerXMLLoader.h"
#include "CGCharacterAnimController.h"
#include "GHEntityHashes.h"
#include "GHStringHashTable.h"
#include "GHPropertyContainer.h"
#include "GHDebugMessage.h"
#include "GHControllerTransition.h"
#include "GHEntity.h"

CGCharacterAnimControllerXMLLoader::CGCharacterAnimControllerXMLLoader(const GHStringHashTable& hashtable,
                                                                       GHControllerMgr& controllerMgr)
: mHashtable(hashtable)
, mControllerMgr(controllerMgr)
{
    
}

void* CGCharacterAnimControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* entity = (GHEntity*)extraData.getProperty(GHEntityHashes::ENTITY);
    
    GHModel* model = entity->mProperties.getProperty(GHEntityHashes::MODEL);
    
    if (!entity || !model)
    {
        GHDebugMessage::outputString("Error loading CGCharacterAnimController. No entity or no model.");
        return 0;
    }
    
    GHIdentifier idleAnim = mHashtable.generateHash("idle");
    GHIdentifier runAnim = mHashtable.generateHash("run");
    GHIdentifier selectedAnim = mHashtable.generateHash("chargeloop");
    GHIdentifier attackAnim = mHashtable.generateHash("meleestrike");
    GHIdentifier deathAnim = mHashtable.generateHash("death1");
    
    return new GHControllerTransition(mControllerMgr, new CGCharacterAnimController(*entity, *model, idleAnim, runAnim, selectedAnim, attackAnim, deathAnim));
}
