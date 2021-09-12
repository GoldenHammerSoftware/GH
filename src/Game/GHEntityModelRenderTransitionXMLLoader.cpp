// Copyright Golden Hammer Software
#include "GHEntityModelRenderTransitionXMLLoader.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHModel.h"
#include "GHModelRenderTransition.h"
#include "GHXMLNode.h"

GHEntityModelRenderTransitionXMLLoader::GHEntityModelRenderTransitionXMLLoader(GHRenderGroup& renderGroup, GHStringIdFactory& hashTable)
: mRenderGroup(renderGroup)
, mIdFactory(hashTable)
{
}

void* GHEntityModelRenderTransitionXMLLoader::create(const GHXMLNode& node,
                                                     GHPropertyContainer& extraData) const
{
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
    if (!parentEnt) {
        GHDebugMessage::outputString("EMRTransition loaded with no parent entity.");
        return 0;
    }

    GHIdentifier modelPropId = GHEntityHashes::MODEL;
    node.readAttrIdentifier("prop", modelPropId, mIdFactory);

    GHModel* parentModel = parentEnt->mProperties.getProperty(modelPropId);
    if (!parentModel) {
        GHDebugMessage::outputString("EMRTransition called with no model.");
        return 0;
    }
    GHIdentifier renderType = 0;
    node.readAttrIdentifier("renderType", renderType, mIdFactory);
    return new GHModelRenderTransition(*parentModel, mRenderGroup, renderType);
}
