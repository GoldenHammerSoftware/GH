// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include <vector>

class GHStateMachine;

// A transition that adds and removes transitions to a state machine.
class GHTransitionChangeTransition : public GHTransition
{
public:
	GHTransitionChangeTransition(GHStateMachine& targetSM, const GHIdentifier& targetState, GHTransition* transition);
	~GHTransitionChangeTransition(void);
	
	virtual void activate(void);
	virtual void deactivate(void);

	// for supporting more than one transition or target state
	void addTargetState(const GHIdentifier& targetState);
	void addTransition(GHTransition* transition);
	
private:
	GHStateMachine& mTargetSM;
	std::vector<GHIdentifier> mTargetStates;
    std::vector<GHTransition*> mTransitions;
	bool mIsActive{ false }; // used to be smarter on deletion.
};
