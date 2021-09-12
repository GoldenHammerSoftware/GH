// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHGUIWidget.h"
#include "GHGUITransitionDesc.h"

class GHControllerMgr;
class GHPropertyContainer;
class GHTimeVal;
class GHGUITransitionControllerFactory;

// A self-destructing controller to add or remove a gui widget
// after waiting for a delay and running a property from 0to1
// this class is meant to be spawned from the gui mgr, and might not work correctly without it.
class GHGUITransitionController : public GHController
{
public:
    GHGUITransitionController(GHControllerMgr& controllerMgr,
                              const GHTimeVal& timeVal,
                              GHGUIWidgetResource* widget,
                              GHGUIWidgetResource* parentWidget,
                              GHPropertyContainer& props,
                              const GHGUITransitionDesc& tDesc,
                              bool activating,
                              GHGUITransitionControllerFactory& factory);
    ~GHGUITransitionController(void);
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);
    
	GHGUIWidgetResource* getWidget(void) { return mWidget; }

	void beginTransitionAnimation(void);
	void endTransitionAnimation(void);

private:
    GHGUITransitionControllerFactory& mFactory;
    GHControllerMgr& mControllerMgr;
    const GHTimeVal& mTimeVal;
    GHGUIWidgetResource* mWidget;
    GHGUIWidgetResource* mParentWidget;
    GHPropertyContainer& mProps;
    GHGUITransitionDesc mDesc;
    // if activating is true we run from 0-1 and then fully activate.
    // else we run from 1 to 0 and then remove renderable.
    bool mActivating;
    // the time at activation
    float mStartTime;
    // have we made it past transitonWait yet
    bool mWaitFinished;
};
