// Copyright Golden Hammer Software
#include "GHStateMachine.h"
#include <set>
#include "GHTransition.h"
#include <algorithm>
#include <assert.h>
#include "GHPlatform/GHDebugMessage.h"

GHStateMachine::StateId GHStateMachine::kInactive = -1;

GHStateMachine::GHStateMachine(void)
: mState(kInactive)
, mExcludedTransitionList(0)
, mIsTransitioning(false)
{
}

GHStateMachine::~GHStateMachine(void)
{
	shutdown();
}

void GHStateMachine::shutdown(void)
{
    setState(kInactive);
    
	// build up a list of total transitions so we don't double delete.
	std::set<GHTransition*> transitions;
	TransitionList::const_iterator listIter;
	std::map<StateId, TransitionList>::iterator mapIter;
	for (mapIter = mStates.begin(); mapIter != mStates.end(); ++mapIter) {
        const TransitionList& currList = mapIter->second;
		for (listIter = currList.begin(); listIter != currList.end(); ++listIter) {
			transitions.insert(*listIter);
		}
	}
	mStates.clear();
	std::set<GHTransition*>::iterator delIter;
	for (delIter = transitions.begin(); delIter != transitions.end(); ++delIter) {
        GHTransition* currTrans = *delIter;
		delete currTrans;
	}
	mStates.clear();
}

void GHStateMachine::addTransition(StateId state, GHTransition* trans)
{
    assert(trans != 0);
    
	// make sure it wasn't already added.
    std::map<StateId, TransitionList>::iterator findIter;
    findIter = mStates.find(state);
    if (findIter != mStates.end())
    {
        TransitionList::iterator listIter;
        for (listIter = findIter->second.begin(); listIter != findIter->second.end(); ++listIter)
        {
            if (*listIter == trans) 
            {
                return;
            }
        }
    }
	
	mStates[state].push_back(trans);
	if (state == mState) {
		trans->activate();
	}
}

void GHStateMachine::removeTransition(GHTransition* trans)
{
	TransitionList::iterator listIter;
	std::map<StateId, TransitionList>::iterator mapIter;
	for (mapIter = mStates.begin(); mapIter != mStates.end(); ++mapIter) {
		for (listIter = mapIter->second.begin(); listIter != mapIter->second.end(); ++listIter) {
			if (*listIter == trans) {
				mapIter->second.erase(listIter);
				if (mapIter->first == mState) {
					trans->deactivate();
				}
				// expect that each transition is only in each state list once
				break;
			}
		}
	}
}

void GHStateMachine::removeTransition(StateId state, GHTransition* trans)
{
    if (state == mState) {
        trans->deactivate();
    }

    std::map<StateId, TransitionList>::iterator findIter;
    findIter = mStates.find(state);
    if (findIter != mStates.end())
    {
        TransitionList::iterator listIter;
        for (listIter = findIter->second.begin(); listIter != findIter->second.end(); ++listIter)
        {
            if (*listIter == trans)
            {
                findIter->second.erase(listIter);
                return;
            }
        }
    }
    GHDebugMessage::outputString("Failed to find transition to remove");
}

void GHStateMachine::setState(StateId state)
{
    if (mIsTransitioning) {
        // don't transition while transitioning, dawg.
        mNextStates.push_back(state);
        return;
    }
    if (state == mState) return;
	mIsTransitioning = true;
    
    std::map<StateId, TransitionList>::iterator stateIter;

    stateIter = mStates.find(state);
    if (stateIter == mStates.end()) mExcludedTransitionList = 0;
    else mExcludedTransitionList = &(stateIter->second);
	deactivateState(mState);
    
    stateIter = mStates.find(mState);
    if (stateIter == mStates.end()) mExcludedTransitionList = 0;
    else mExcludedTransitionList = &(stateIter->second);
	activateState(state);

	mExcludedTransitionList = 0;
	mState = state;
    mIsTransitioning = false;
    
    if (mNextStates.size()) {
        std::list<StateId> nextStatesCopy = mNextStates;
        mNextStates.clear();
        std::list<StateId>::iterator nextStatesIter;
        for (nextStatesIter = nextStatesCopy.begin(); nextStatesIter != nextStatesCopy.end(); ++nextStatesIter)
        {
            StateId nextState = *nextStatesIter;
            setState(nextState);
        }
    }
}

void GHStateMachine::deactivateState(StateId state)
{
    std::map<StateId, TransitionList>::iterator findIter;
    findIter = mStates.find(state);
    if (findIter == mStates.end()) return;
	const TransitionList& oldList = findIter->second;

	TransitionList::const_iterator listIter;
	for (listIter = oldList.begin(); listIter != oldList.end(); ++listIter) {
		if (!excluded(*listIter)) {
			(*listIter)->deactivate();
		}
	}
}

void GHStateMachine::activateState(StateId state)
{
    std::map<StateId, TransitionList>::iterator findIter;
    findIter = mStates.find(state);
    if (findIter == mStates.end()) return;
	const TransitionList& newList = findIter->second;

	TransitionList::const_iterator listIter;
	for (listIter = newList.begin(); listIter != newList.end(); ++listIter) 
    {
        GHTransition* currTransition = *listIter;
		if (!excluded(currTransition)) {
			(currTransition)->activate();
		}
	}
}

bool GHStateMachine::excluded(const GHTransition* trans) const
{
	return mExcludedTransitionList &&
	std::find(mExcludedTransitionList->begin(), mExcludedTransitionList->end(), trans) != mExcludedTransitionList->end();
}
