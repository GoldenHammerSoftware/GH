// Copyright Golden Hammer Software
#include "GHGUIWidgetTransitionXMLLoader.h"
#include "GHGUIWidgetTransition.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHGUITransitionDescXMLLoader.h"
#include "GHGUITransitionDesc.h"

GHGUIWidgetTransitionXMLLoader::GHGUIWidgetTransitionXMLLoader(GHGUIMgr& guiMgr, GHResourceFactory& resFactory,
                                                               GHXMLObjFactory& objFactory,
                                                               const GHGUITransitionDescXMLLoader& tDescLoader)
: mGUIMgr(guiMgr)
, mResFactory(resFactory)
, mObjFactory(objFactory)
, mTDescLoader(tDescLoader)
{    
}

void* GHGUIWidgetTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIWidgetResource* widget = 0;

	GHGUITransitionDesc tDesc;
	mTDescLoader.populate(&tDesc, node, extraData);

    const char* panelName = node.getAttribute("widget");
    if (panelName) {
        widget = (GHGUIWidgetResource*)mResFactory.getCacheResource(panelName);
    }
    else if (node.getChildren().size())
    {
        widget = (GHGUIWidgetResource*)mObjFactory.load(*node.getChildren()[0], &extraData);
    }
    if (!widget) {
        GHDebugMessage::outputString("Can't load panel for widget transition");
        return 0;
    }
    
    bool addToParent = false;
    node.readAttrBool("addToParent", addToParent);
    GHGUIWidget* parentWidget = 0;
    if (addToParent) {
        parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
    }
	float priority = 0;
	node.readAttrFloat("priority", priority);
	unsigned int category = 0;
	node.readAttrUInt("category", category);

    bool addToStack = true;
    node.readAttrBool("addToStack", addToStack);

	if (parentWidget)
	{
		return new GHGUIWidgetTransition(widget, parentWidget, &mGUIMgr, tDesc, addToStack);
	}
	return new GHGUIWidgetTransition(widget, &mGUIMgr, tDesc, addToStack, priority, category);
}
