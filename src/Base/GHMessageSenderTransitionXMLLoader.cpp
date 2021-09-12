// Copyright Golden Hammer Software
#include "GHMessageSenderTransitionXMLLoader.h"
#include "GHXMLNode.h"
#include "GHMessageXMLLoader.h"
#include "GHUtils/GHMessageSenderTransition.h"
#include "GHPlatform/GHDebugMessage.h"

GHMessageSenderTransitionXMLLoader::GHMessageSenderTransitionXMLLoader(const GHMessageXMLLoader& messageLoader,
                                                                       GHMessageHandler& messageHandler)
: mMessageLoader(messageLoader)
, mMessageHandler(messageHandler)
{
}

void* GHMessageSenderTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMessageSenderTransition* ret = new GHMessageSenderTransition(mMessageHandler);
    populate(ret, node, extraData);
    return ret;
}

void GHMessageSenderTransitionXMLLoader::populate(void* obj, const GHXMLNode& node, 
                                                  GHPropertyContainer& extraData) const
{
    GHMessageSenderTransition* ret = (GHMessageSenderTransition*)obj;
    const GHXMLNode::NodeList& childNodes = node.getChildren();
    for (size_t i = 0; i < childNodes.size(); ++i)
    {
        GHMessage* message = (GHMessage*)mMessageLoader.create(*childNodes[i], extraData);
        if (!message) {
            GHDebugMessage::outputString("Unable to load message child for sender.");
        }
        else 
        {
            bool deactivate = false;
            childNodes[i]->readAttrBool("deactivate", deactivate);

            if (deactivate)
            {
                ret->addDeactivateMessage(message);
            }
            else
            { 
                ret->addActivateMessage(message);
            }
        }
    }
}
