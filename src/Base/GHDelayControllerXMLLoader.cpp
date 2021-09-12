// Copyright Golden Hammer Software
#include "GHDelayControllerXMLLoader.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHDelayController.h"
#include "GHPlatform/GHDebugMessage.h"

GHDelayControllerXMLLoader::GHDelayControllerXMLLoader(const GHTimeVal& time, GHXMLObjFactory& objFactory)
: mTime(time)
, mObjFactory(objFactory)
{
}

void* GHDelayControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (!node.getChildren().size())
	{
		GHDebugMessage::outputString("No transitions specified for delay controller");
		return 0;
	}
	GHTransition* trans = (GHTransition*)mObjFactory.load(*node.getChildren()[0], &extraData);
	if (!trans)
	{
		GHDebugMessage::outputString("Failed to load transition for delay controller");
		return 0;
	}
	float delayTime = 0.0f;
	node.readAttrFloat("time", delayTime);
	GHDelayController* ret = new GHDelayController(mTime, delayTime, trans);
	return ret;
}
