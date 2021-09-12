// Copyright Golden Hammer Software
#include "GHGUITransitionControllerFactory.h"
#include "GHGUITransitionController.h"
#include "GHUtils/GHControllerMgr.h"

GHGUITransitionControllerFactory::GHGUITransitionControllerFactory(GHControllerMgr& controllerMgr,
                                                                   const GHTimeVal& timeVal,
                                                                   GHPropertyContainer& props)
: mControllerMgr(controllerMgr)
, mTimeVal(timeVal)
, mProps(props)
, mDeleting(false)
{
}

GHGUITransitionControllerFactory::~GHGUITransitionControllerFactory(void)
{
    mDeleting = true;
    for (size_t i = 0; i < mSpawned.size(); ++i) {
        mControllerMgr.remove(mSpawned[i].second);
        delete mSpawned[i].second;
    }
    for (size_t i = 0; i < mQueued.size(); ++i) {
        delete mQueued[i].second;
    }
}

void GHGUITransitionControllerFactory::spawnTransition(GHGUIWidgetResource* widget,
                                                       GHGUIWidgetResource* parentWidget,
                                                       const GHGUITransitionDesc& tDesc,
                                                       bool activating)
{
    GHGUITransitionController* ret = new GHGUITransitionController(mControllerMgr, mTimeVal, widget, parentWidget, mProps, tDesc, activating, *this);
    
    // see if we are already transitioning this resource, if so queue, otherwise spawn.
    bool alreadyExists = false;
    TransitionList::iterator findIter;
    for (findIter = mSpawned.begin(); findIter != mSpawned.end(); ++findIter)
    {
        if (findIter->first == widget) {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        mControllerMgr.add(ret);
        mSpawned.push_back(TransitionPair(widget, ret));
    }
    else
    {
        mQueued.push_back(TransitionPair(widget, ret));
    }
}

void GHGUITransitionControllerFactory::abortTransitions(GHGUIWidgetResource* widget)
{
	// keep a list of all transitions to delete until after we have cleared out the lists.
	// otherwise deleting calls unregister which can mess with our iteration.
	std::vector<GHGUITransitionController*> transitionsToDelete;

	// first clear out any pending.  otherwise deleting a spawned could add a spawned from pending.
	TransitionList::iterator findIter;
	bool findPending = true;
	while (findPending)
	{
		// if we make it through the list without finding any then we can move on.
		findPending = false;

		for (findIter = mQueued.begin(); findIter != mQueued.end(); ++findIter)
		{
			if (findIter->first == widget) 
			{
				findPending = true;
				transitionsToDelete.push_back(findIter->second);
				mQueued.erase(findIter);
				break;
			}
		}
	}
	// then clear out any active transitions.
	findPending = true;
	while (findPending)
	{
		// if we make it through the list without finding any then we can move on.
		// djw does not expect things to be in this list twice anyway.
		findPending = false;

		for (findIter = mSpawned.begin(); findIter != mSpawned.end(); ++findIter)
		{
			if (findIter->first == widget)
			{
				findPending = true;
				findIter->second->endTransitionAnimation();
				transitionsToDelete.push_back(findIter->second);
				mSpawned.erase(findIter);
				break;
			}
		}
	}

	for (auto delIter = transitionsToDelete.begin(); delIter != transitionsToDelete.end(); ++delIter)
	{
		delete* delIter;
	}
}

void GHGUITransitionControllerFactory::unregister(GHGUITransitionController* trans)
{
    if (mDeleting) return;
    GHGUIWidgetResource* widget = 0;
    TransitionList::iterator findIter;
    for (findIter = mSpawned.begin(); findIter != mSpawned.end(); ++findIter)
    {
        if (findIter->second == trans) {
            widget = findIter->first;
            mSpawned.erase(findIter);
            break;
        }
    }
    if (!widget) return;
    for (findIter = mQueued.begin(); findIter != mQueued.end(); ++findIter)
    {
        if (findIter->first == widget)
        {
            mControllerMgr.add(findIter->second);
            mSpawned.push_back(TransitionPair(widget, findIter->second));
            mQueued.erase(findIter);
            break;
        }
    }
}

int GHGUITransitionControllerFactory::calcDepth(int startDepth)
{
	for (unsigned int i = 0; i < mSpawned.size(); ++i) {
		startDepth += mSpawned[i].first->get()->calcDepth(startDepth);
	}
	return startDepth;
}

