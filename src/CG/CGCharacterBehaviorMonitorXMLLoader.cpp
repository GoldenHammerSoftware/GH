// Copyright 2010 Golden Hammer Software
#include "CGCharacterBehaviorMonitorXMLLoader.h"
#include "CGCharacterBehaviorMonitor.h"
#include "GHXMLNode.h"
#include "GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHControllerTransition.h"
#include "GHEntity.h"

CGCharacterBehaviorMonitorXMLLoader::CGCharacterBehaviorMonitorXMLLoader(const GHTimeVal& timeVal,
                                                                         GHControllerMgr& controllerMgr)
: mTimeVal(timeVal)
, mControllerMgr(controllerMgr)
{
    
}

void* CGCharacterBehaviorMonitorXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* entity = (GHEntity*)extraData.getProperty(GHEntityHashes::ENTITY);
    if (!entity) { return 0; }
    
    GHModel* model = entity->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!model) { return 0; }
    
    return new GHControllerTransition(mControllerMgr, new CGCharacterBehaviorMonitor(mTimeVal, *entity, *model));
}
