// Copyright Golden Hammer Software
#include "GHEntityModelAnimTransitionXMLLoader.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHModel.h"
#include "GHString/GHStringIdFactory.h"
#include "GHString/GHHashListParser.h"
#include "GHXMLNode.h"
#include "GHModelAnimTransition.h"

GHEntityModelAnimTransitionXMLLoader::GHEntityModelAnimTransitionXMLLoader(const GHStringIdFactory& hashTable, const GHTimeVal& time)
: mIdFactory(hashTable)
, mTime(time)
{
}

void* GHEntityModelAnimTransitionXMLLoader::create(const GHXMLNode& node, 
                                                  GHPropertyContainer& extraData) const
{
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
    if (!parentEnt) {
        GHDebugMessage::outputString("EMATransition loaded with no parent entity.");
        return 0;
    }
    GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!parentModel) {
        GHDebugMessage::outputString("EMATransition called with no model.");
        return 0;
    }
    const char* animNames = node.getAttribute("anim");
    if (!animNames) {
        GHDebugMessage::outputString("No anim attribute in transition loader.");
        return 0;
    }
    std::vector<GHIdentifier> anims;
    GHHashListParser::parseHashList(animNames, mIdFactory, anims);
    if (!anims.size()) {
        GHDebugMessage::outputString("Empty anim transition loader.");
        return 0;
    }
    float timeOffset = 0;
    node.readAttrFloat("randTimeOffset", timeOffset);

    GHModelAnimTransition* ret = new GHModelAnimTransition(*parentModel, anims[0], mTime, timeOffset);
    for (size_t i = 1; i < anims.size(); ++i) ret->addAnim(anims[i]);
    return ret;
}


