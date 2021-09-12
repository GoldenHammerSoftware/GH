// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHGUIWidget.h"

class GHControllerMgr;
class GHTimeVal;
class GHPropertyContainer;
class GHGUIMgr;
class GHGUIWidget;
class GHGUITransitionController;
class GHGUITransitionDesc;

// A widget to wrap the stuff needed to make a GHGUITransitionController
class GHGUITransitionControllerFactory
{
public:
    GHGUITransitionControllerFactory(GHControllerMgr& controllerMgr,
                                     const GHTimeVal& timeVal,
                                     GHPropertyContainer& props);
    ~GHGUITransitionControllerFactory(void);
    
    void spawnTransition(GHGUIWidgetResource* widget,
                         GHGUIWidgetResource* parentWidget,
                         const GHGUITransitionDesc& tDesc,
                         bool activating);
	// kill off any active and pending transitions for a widget.
	// this prevents popping a widget while it is transitioning in, which puts the gui in a bad state.
	void abortTransitions(GHGUIWidgetResource* widget);
    
    // let us know that a transition is done so we can remove it from out deletion list.
    void unregister(GHGUITransitionController* trans);
    
	// assign depth values to transitioning guis.
	int calcDepth(int startDepth);

private:
    GHControllerMgr& mControllerMgr;
    const GHTimeVal& mTimeVal;
    GHPropertyContainer& mProps;
    // we keep a list of the spawned and active transitions,
    //  so that on delete we can remove them from the controller manager.
    typedef std::pair<GHGUIWidgetResource*, GHGUITransitionController*> TransitionPair;
    typedef std::vector<TransitionPair> TransitionList;
    // the list of transitions we have spawned.
    TransitionList mSpawned;
    // the list of transitions we have queued for spawning.
    // whenever a transition finishes we look for a queued transition with the same widget resource.
    TransitionList mQueued;
    // a flag for during delete so that we don't get repeat unregisters.
    bool mDeleting;
};
