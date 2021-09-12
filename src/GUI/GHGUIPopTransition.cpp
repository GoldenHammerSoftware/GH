// Copyright Golden Hammer Software
#include "GHGUIPopTransition.h"
#include "GHGUIMgr.h"

GHGUIPopTransition::GHGUIPopTransition(GHGUIMgr& guiMgr, const GHIdentifier& id, const GHGUITransitionDesc& tDesc, 
									   size_t category, bool addToStack)
: mGUIMgr(guiMgr)
, mId(id)
, mDesc(tDesc)
, mAddToStack(addToStack)
, mCategory(category)
{
}

void GHGUIPopTransition::activate(void)
{
    mGUIMgr.popWidget(mId, mDesc, mCategory, mAddToStack);
}
