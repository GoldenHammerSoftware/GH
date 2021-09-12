// Copyright Golden Hammer Software
#include "GHMessageHandlerTransitionXMLLoader.h"
#include "GHUtils/GHEventMgr.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessageHandlerTransition.h"

GHMessageHandlerTransitionXMLLoader::GHMessageHandlerTransitionXMLLoader(GHEventMgr& eventMgr, 
	const GHStringIdFactory& idFactory, const GHXMLObjFactory& objFactory)
	: mEventMgr(eventMgr)
	, mIdFactory(idFactory)
	, mObjFactory(objFactory)
{
}

/*
<messageHandler message="id">
	// handler xml here.
</messageHandlerTransition>
*/
void* GHMessageHandlerTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (node.getChildren().size() == 0)
	{
		GHDebugMessage::outputString("No transition xml specified for message handler transition");
		return 0;
	}
	const char* messageIdStr = node.getAttribute("message");
	if (!messageIdStr)
	{
		GHDebugMessage::outputString("No message id specified for message handler transition");
		return 0;
	}
	GHIdentifier messageId = mIdFactory.generateHash(messageIdStr);

	GHMessageHandler* handler = (GHMessageHandler*)mObjFactory.load(*node.getChildren()[0], &extraData);
	if (!handler)
	{
		GHDebugMessage::outputString("Failed to load handler for message handler transition");
		return 0;
	}

	GHMessageHandlerTransition* ret = new GHMessageHandlerTransition(mEventMgr, messageId, handler);
	return ret;
}
