// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHGUIWidget.h"
#include "GHGUITransitionDesc.h"

class GHGUIMgr;

// A transition for showing a gui panel while this transition is active.
// If targetParent is specified, always add/remove from that widget.
// If transition time or wait is specified, we run an animation as we add or remove.
class GHGUIWidgetTransition : public GHTransition
{
public:
    GHGUIWidgetTransition(GHGUIWidgetResource* widget, GHGUIMgr* guiMgr,
		const GHGUITransitionDesc& tDesc, bool addToStack, float priority, size_t category);
	GHGUIWidgetTransition(GHGUIWidgetResource* widget, GHGUIWidget* targetParent, GHGUIMgr* guiMgr,
		const GHGUITransitionDesc& tDesc, bool addToStack);
	~GHGUIWidgetTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHGUIMgr* mGUIMgr;
    GHGUIWidgetResource* mWidget;
    GHGUIWidget* mTargetParent;
	GHGUITransitionDesc mTDesc;
	bool mAddToStack;
	float mPriority;
	size_t mCategory;
};
