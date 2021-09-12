// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include <map>
#include <list>

class GHTransition;

class GHStateMachine
{
public:
    typedef unsigned long StateId;
    // setState kInactive is a way to disable all transitions.
    static StateId kInactive;
    
public:
	GHStateMachine(void);
	~GHStateMachine(void);

	void shutdown(void);
	
	// take ownership of a transition and assign it to a state.
	// it is safe to add a transition to more than one state.
	void addTransition(StateId state, GHTransition* trans);
	// remove a transition from all states.
	void removeTransition(GHTransition* trans);
	// remove a transition from one state if found.
	void removeTransition(StateId state, GHTransition* trans);
	
	// transition to a new state.  call deactivate on previous transitions,
	//  and activate on the new transitions.
	void setState(StateId state);
	StateId getState(void) const { return mState; }
	
private:
	void deactivateState(StateId state);
	void activateState(StateId state);
	bool excluded(const GHTransition* trans) const;
	
private:
	// set of transitions that are active in each state.
	typedef std::vector<GHTransition*> TransitionList;
	std::map<StateId, TransitionList> mStates;
	TransitionList* mExcludedTransitionList;
	// the currently active state.
	StateId mState;	
    
    // safety for switching to a new state while activating an old state
    bool mIsTransitioning;
    std::list<StateId> mNextStates;
};
