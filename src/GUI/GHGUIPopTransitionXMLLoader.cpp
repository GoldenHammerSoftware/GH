// Copyright Golden Hammer Software
#include "GHGUIPopTransitionXMLLoader.h"
#include "GHGUIPopTransition.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUITransitionDescXMLLoader.h"

GHGUIPopTransitionXMLLoader::GHGUIPopTransitionXMLLoader(GHGUIMgr& guiMgr, const GHStringIdFactory& hashTable,
														 const GHGUITransitionDescXMLLoader& tDescLoader)
: mGUIMgr(guiMgr)
, mIdFactory(hashTable)
, mTDescLoader(tDescLoader)
{
}

void* GHGUIPopTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUITransitionDesc desc;
	mTDescLoader.populate(&desc, node, extraData);
    bool addToStack = true;
    node.readAttrBool("addToStack", addToStack);
    
    const char* panelName = node.getAttribute("widget");
    if (!panelName) {
        GHDebugMessage::outputString("No panel specified for pop transition");
        return 0;
    }

	unsigned int category = 0;
	node.readAttrUInt("category", category);

    GHIdentifier panelId = mIdFactory.generateHash(panelName);
    return new GHGUIPopTransition(mGUIMgr, panelId, desc, category, addToStack);
}
