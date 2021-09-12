// Copyright Golden Hammer Software
#include "GHGUISizeTransitionXMLLoader.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHGUISizeTransition.h"

GHGUISizeTransitionXMLLoader::GHGUISizeTransitionXMLLoader(void)
{
}

void* GHGUISizeTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIWidgetResource* parentRes = (GHGUIWidgetResource*)extraData.getProperty(GHGUIProperties::GP_PARENTWIDGETRES);
    if (!parentRes) {
		GHDebugMessage::outputString("No parent specified for size transition");
		return 0;
	}
	float sizeMult = 1.0f;
	node.readAttrFloat("sizeMult", sizeMult);
	if (!sizeMult) {
		GHDebugMessage::outputString("size change transition would divide by 0");
		return 0;
	}
	GHGUISizeTransition* ret = new GHGUISizeTransition(*parentRes->get(), sizeMult);
	return ret;
}
