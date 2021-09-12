// Copyright Golden Hammer Software
#include "GHGUIWidgetTransition.h"
#include "GHGUIMgr.h"

GHGUIWidgetTransition::GHGUIWidgetTransition(GHGUIWidgetResource* widget, GHGUIMgr* guiMgr,
	const GHGUITransitionDesc& tDesc, bool addToStack, float priority, size_t category)
	: mGUIMgr(guiMgr)
	, mWidget(0)
	, mTargetParent(0)
	, mTDesc(tDesc)
	, mAddToStack(addToStack)
	, mPriority(priority)
	, mCategory(category)
{
	assert(widget != 0);
	assert(guiMgr);

	GHRefCounted::changePointer((GHRefCounted*&)mWidget, widget);
}

GHGUIWidgetTransition::GHGUIWidgetTransition(GHGUIWidgetResource* widget, GHGUIWidget* targetParent, GHGUIMgr* guiMgr,
	const GHGUITransitionDesc& tDesc, bool addToStack)
	: mGUIMgr(guiMgr)
	, mWidget(0)
	, mTargetParent(targetParent)
	, mTDesc(tDesc)
	, mAddToStack(addToStack)
	, mPriority(0)
	, mCategory(0)
{
	assert(widget != 0);
	assert(targetParent);

	GHRefCounted::changePointer((GHRefCounted*&)mWidget, widget);
}

GHGUIWidgetTransition::~GHGUIWidgetTransition(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mWidget, 0);
}

void GHGUIWidgetTransition::activate(void)
{
	if (mTargetParent) {
		mTargetParent->addChild(mWidget, mPriority);
	}
	else if (mGUIMgr) {
		mGUIMgr->pushWidget(mWidget, mTDesc, mPriority, mCategory);
	}
}

void GHGUIWidgetTransition::deactivate(void)
{
    if (mTargetParent) mTargetParent->removeChild(mWidget);
	else if (mGUIMgr) mGUIMgr->popWidget(mWidget, mTDesc, mCategory, mAddToStack);
}

