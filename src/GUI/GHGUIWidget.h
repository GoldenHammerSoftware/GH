// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHRect.h"
#include "GHUtils/GHResource.h"
#include "GHUtils/GHStateMachine.h"
#include "GHGUIPosDesc.h"
#include "GHString/GHIdentifier.h"
#include <vector>
#include "GHGUIChildList.h"
#include "GHUtils/GHPropertyContainer.h"

class GHGUIRectMaker;

// interface for something that can be in the gui
class GHGUIWidget
{
public:
	struct WidgetGroup
	{
		bool mIsActive{ false };
		GHGUIChildList mWidgets;
	};

    bool debugging{ false };

public:
    GHGUIWidget(const GHGUIRectMaker& rectMaker);
    virtual ~GHGUIWidget(void);

    // set widget state to WS_TRANSITIONIN and show renderable
    void enterTransitionIn(void);
    // set widget state to WS_ACTIVE
    void finishTransitionIn(void);
    // set widget state to WS_TRANSITIONOUT
    void enterTransitionOut(void);
    // set widget state to WS_NONE and hide renderable
    void finishTransitionOut(void);
    
    // do any necessary work after a parent or local position has changed
    virtual void updatePosition(void);
    
    // called by panel when adding/removing a child.
	void setParent(const GHRect<float, 2>* parent, const GHGUIWidget* parentWidget);
    const GHRect<float,2>* getParent(void) const { return mParent; }
	const GHGUIWidget* getParentWidget(void) const { return mParentWidget; }

	GHPropertyContainer& getPropertyContainer(void) { return mPropertyContainer; }
	const GHPropertyContainer& getPropertyContainer(void) const { return mPropertyContainer; }

    void setPosDesc(const GHGUIPosDesc& desc);
    const GHGUIPosDesc& getPosDesc(void) const { return mPosDesc; }
    const GHRect<float,2>& getScreenPos(void) const { return mScreenPos; }
    
    GHStateMachine& getWidgetStates(void) { return mWidgetStates; }
    
    void setId(const GHIdentifier& id) { mId = id; }
    const GHIdentifier& getId(void) const { return mId; }
    
    void addChild(GHResourcePtr<GHGUIWidget>* child, float priority = 0);
    void removeChild(GHResourcePtr<GHGUIWidget>* child);
    GHResourcePtr<GHGUIWidget>* getChild(const GHIdentifier& id);
	const GHResourcePtr<GHGUIWidget>* getChild(const GHIdentifier& id) const;
    size_t getNumChildren(void) const;
    GHResourcePtr<GHGUIWidget>* getChildAtIndex(size_t index);
	const GHResourcePtr<GHGUIWidget>* getChildAtIndex(size_t index) const;
    
    // assign an mDepth value to self and children based on prevdepth
    // return maxDepth
    int calcDepth(int prevDepth);
	int getDepth(void) const { return mDepth; }
    int findDepthOfDeepestChild(void) const;

    void setPixelSizeMod(float val) { mPixelSizeMod = val; }
    // recurse upwards calculating a pixel size mod.
    float calcPixelSizeMod(void) const;

	void setHidden(bool val) { mHidden = val; }
	bool isHidden(void) const { return mHidden; }

protected:
    virtual void onEnterTransitionIn(void) = 0;
    virtual void onFinishTransitionIn(void) = 0;
    virtual void onEnterTransitionOut(void) = 0;
    virtual void onFinishTransitionOut(void) = 0;
    // function called if calcDepth changes mDepth.
    virtual void onDepthChange(void) {}
    
protected:
    const GHGUIRectMaker& mRectMaker;
    const GHRect<float,2>* mParent;
    const GHGUIWidget* mParentWidget;
    GHStateMachine mWidgetStates;
	GHPropertyContainer mPropertyContainer;
    // the info we use to decide where we are on screen
    GHGUIPosDesc mPosDesc;
    // the calculated value of where we are.
    GHRect<float,2> mScreenPos;
    // how far we are in iterating the tree.  used for sorting controllers.
    int mDepth;
    // a modifier to pixel size for us and our children.
    float mPixelSizeMod;

	// flag to say we are hidden, which means we are possibly attached to a parent but not active.
	// we expect hidden items to remain in inactive state.
	bool mHidden;
    
private:
    GHIdentifier mId;
	GHGUIChildList mChildren;
};

typedef GHResourcePtr<GHGUIWidget> GHGUIWidgetResource;
