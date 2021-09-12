// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHGUIWidget.h"
#include "GHString/GHString.h"
#include "GHGUITransitionDesc.h"

class GHGUIMgr;
class GHResourceFactory;

// A transition to push a widget when activated.  leaves it there on deactivate
// If transition time or wait is specified, we run an animation as we add or remove.
class GHGUIPushTransition : public GHTransition
{
public:
    GHGUIPushTransition(GHGUIMgr& guiMgr, GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, float priority, size_t category);
    GHGUIPushTransition(GHGUIMgr& guiMgr, GHResourceFactory& resourceFactory, const char* resourceName,
                        const GHGUITransitionDesc& tDesc, float priority, size_t category, bool keepLoaded);
    ~GHGUIPushTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    GHGUIMgr& mGUIMgr;
    GHGUIWidgetResource* mWidget;
	GHGUITransitionDesc mTDesc;
	float mPriority;
	size_t mCategory;

    GHResourceFactory* mResourceFactory;
    GHString mResourceName;
    // a flag to say whether we keep our gui loaded once we create it.
    // only applies if we are created with a resource name and not a widget.
    bool mKeepLoaded;
};
