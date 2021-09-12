// Copyright Golden Hammer Software
#include "GHGUISizeOscillateTransitionXMLLoader.h"
#include "GHGUISizeOscillater.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHXMLNode.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUISizeOscillateTransitionXMLLoader::GHGUISizeOscillateTransitionXMLLoader(const GHTimeVal& time, GHControllerMgr& controllerMgr)
: mTime(time)
, mControllerMgr(controllerMgr)
{
}

void* GHGUISizeOscillateTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidgetResource* parentRes = (GHGUIWidgetResource*)extraData.getProperty(GHGUIProperties::GP_PARENTWIDGETRES);
	if (!parentRes) {
		GHDebugMessage::outputString("No parent specified for oscillate transition");
		return 0;
	}

	float changeSize = 0.5f;
	float changeSpeed = 1.0f;
	node.readAttrFloat("max", changeSize);
	node.readAttrFloat("speed", changeSpeed);
	GHGUISizeOscillater* bounce = new GHGUISizeOscillater(*parentRes->get(), mTime, changeSize, changeSpeed);
	GHControllerTransition* ret = new GHControllerTransition(mControllerMgr, bounce);
	return ret;
}
