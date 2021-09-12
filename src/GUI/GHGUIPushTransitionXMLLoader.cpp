// Copyright Golden Hammer Software
#include "GHGUIPushTransitionXMLLoader.h"
#include "GHGUIPushTransition.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHXMLObjFactory.h"
#include "GHGUITransitionDescXMLLoader.h"
#include "GHGUITransitionDesc.h"

GHGUIPushTransitionXMLLoader::GHGUIPushTransitionXMLLoader(GHGUIMgr& guiMgr, 
                                                           GHResourceFactory& resourceFactory,
                                                           GHXMLObjFactory& objFactory,
														   const GHGUITransitionDescXMLLoader& tDescLoader)
: mGUIMgr(guiMgr)
, mResourceFactory(resourceFactory)
, mObjFactory(objFactory)
, mTDescLoader(tDescLoader)
{
}

void* GHGUIPushTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUITransitionDesc tDesc;
	mTDescLoader.populate(&tDesc, node, extraData);
    float priority = 0;
	node.readAttrFloat("priority", priority);
	unsigned int category = 0;
	node.readAttrUInt("category", category);

    const char* panelName = node.getAttribute("widget");
    if (!panelName) {
        // if no panel name specified, we expect to have a child that is a widget
        if (!node.getChildren().size())
        {
            GHDebugMessage::outputString("No panel specified for push transition");
            return 0;
        }
        GHGUIWidgetResource* widget = (GHGUIWidgetResource*)mObjFactory.load(*node.getChildren()[0], &extraData);
        if (!widget) {
            GHDebugMessage::outputString("Can't load child widget for push transition.");
            return 0;
        }
        return new GHGUIPushTransition(mGUIMgr, widget, tDesc, priority, category);
    }
    
    bool loadOnDemand = true;
    node.readAttrBool("loadOnDemand", loadOnDemand);
    if (loadOnDemand)
    {
        bool keepLoaded = false;
        node.readAttrBool("keepLoaded", keepLoaded);
        return new GHGUIPushTransition(mGUIMgr, mResourceFactory, panelName, tDesc, priority, category, keepLoaded);
    }
    
    GHGUIWidgetResource* widget = (GHGUIWidgetResource*)mResourceFactory.getCacheResource(panelName);
    if (!widget) {
        GHDebugMessage::outputString("Can't load panel for push transition");
        return 0;
    }
    return new GHGUIPushTransition(mGUIMgr, widget, tDesc, priority, category);
}
