// Copyright Golden Hammer Software
#include "GHGUIWidgetMessageTransitionXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHMessageSenderTransition.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIWidget.h"
#include "GHGUIProperties.h"

GHGUIWidgetMessageTransitionXMLLoader::GHGUIWidgetMessageTransitionXMLLoader(GHStringIdFactory& hashTable, GHMessageHandler& messageHandler)
: mIdFactory(hashTable)
, mMessageHandler(messageHandler)
{
}

void* GHGUIWidgetMessageTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidgetResource* parentRes = (GHGUIWidgetResource*)extraData.getProperty(GHGUIProperties::GP_PARENTWIDGETRES);
	if (!parentRes) {
		GHDebugMessage::outputString("No parent specified for gui widget message transition");
		return 0;
	}

	const char* messageStr = node.getAttribute("messageId");
	const char* propStr = node.getAttribute("propId");
	if (!messageStr || !propStr)
	{
		GHDebugMessage::outputString("wrong arguments for gui widget message");
		return 0;
	}
	GHIdentifier messageId = mIdFactory.generateHash(messageStr);
	GHIdentifier propId = mIdFactory.generateHash(propStr);

	GHMessageSenderTransition* ret = new GHMessageSenderTransition(mMessageHandler);
	GHMessage* message = new GHMessage(messageId);
	message->getPayload().setProperty(propId, parentRes);
	ret->addActivateMessage(message);
	return ret;
}
