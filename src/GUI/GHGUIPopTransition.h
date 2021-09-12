// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include "GHGUITransitionDesc.h"
#include <cstddef>

class GHGUIMgr;

// A transition to pop a widget when activated.  
// If transition time or wait is specified, we run an animation as we add or remove.
class GHGUIPopTransition : public GHTransition
{
public:
    GHGUIPopTransition(GHGUIMgr& guiMgr, const GHIdentifier& id,
                       const GHGUITransitionDesc& tDesc,
					   size_t category,
                       bool addToStack);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    GHGUIMgr& mGUIMgr;
    GHIdentifier mId;
    GHGUITransitionDesc mDesc;
	size_t mCategory;
    bool mAddToStack;
};
