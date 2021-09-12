// Copyright Golden Hammer Software
#include "GHGUIAnimXMLLoader.h"
#include "GHString/GHStringIdFactory.h"
#include "GHGUIPosDescXMLLoader.h"
#include "GHGUIAnim.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"

GHGUIAnimXMLLoader::GHGUIAnimXMLLoader(const GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                                       const GHGUIPosDescXMLLoader& posLoader)
: mProps(props)
, mIdFactory(hashTable)
, mPosLoader(posLoader)
{
}

void* GHGUIAnimXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    if (!node.getChildren().size()) {
        GHDebugMessage::outputString("No frames specified for gui anim");
        return 0;
    }
    GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
    if (!parentWidget) {
        GHDebugMessage::outputString("Failed to find parent widget for anim.");
        return 0;
    }

    const char* propStr = node.getAttribute("prop");
    GHIdentifier propId = mIdFactory.generateHash(propStr);
    
    std::vector<GHGUIAnim::Frame> frames;
    for (size_t i = 0; i < node.getChildren().size(); ++i) {
        const GHXMLNode* currNode = node.getChildren()[i];
        if (!currNode->getChildren().size()) {
            continue;
        }
        GHGUIAnim::Frame currFrame;
        currNode->readAttrFloat("val", currFrame.mPropVal);
        mPosLoader.populate(&currFrame.mPos, *currNode->getChildren()[0], extraData);
        frames.push_back(currFrame);
    }
    if (!frames.size()) {
        GHDebugMessage::outputString("No frames loaded for gui anim");
        return 0;
    }
    
    GHGUIAnim* ret = new GHGUIAnim(propId, mProps, frames, *parentWidget);
    return ret; 
}
