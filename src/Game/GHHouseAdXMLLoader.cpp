// Copyright Golden Hammer Software
#include "GHHouseAdXMLLoader.h"
#include "GHGUIButtonXMLLoader.h"
#include "GHHouseAd.h"
#include "GHXMLNode.h"

GHHouseAdXMLLoader::GHHouseAdXMLLoader(const GHGUIButtonXMLLoader& buttonLoader,
                                       GHMessageHandler& messageQueue, GHIdentifier houseAdTransId)
: mButtonLoader(buttonLoader)
, mMessageQueue(messageQueue)
, mTransitionId(houseAdTransId)
{
}

void* GHHouseAdXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHHouseAd* ret = new GHHouseAd(mMessageQueue, mTransitionId);
    
    const GHXMLNode::NodeList& children = node.getChildren();
    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* child = children[i];
        GHGUIWidgetResource* button = (GHGUIWidgetResource*)mButtonLoader.create(*child, extraData);
        ret->addAd(button);
    }
    return ret;
}
