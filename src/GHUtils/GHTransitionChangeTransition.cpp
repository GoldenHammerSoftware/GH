// Copyright Golden Hammer Software
#include "GHTransitionChangeTransition.h"
#include "GHUtils/GHStateMachine.h"

GHTransitionChangeTransition::GHTransitionChangeTransition(GHStateMachine& targetSM, const GHIdentifier& targetState, GHTransition* transition)
	: mTargetSM(targetSM)
{
	mTransitions.push_back(transition);
	mTargetStates.push_back(targetState);
}

GHTransitionChangeTransition::~GHTransitionChangeTransition(void)
{
	// If these transitions are still owned by a state machine they will be double deleted.
	if (!mIsActive)
	{
		for (size_t i = 0; i < mTransitions.size(); ++i)
		{
			delete mTransitions[i];
		}
	}
}

void GHTransitionChangeTransition::activate(void)
{
	mIsActive = true;
	for (size_t transitionIter = 0; transitionIter < mTransitions.size(); ++transitionIter)
	{
		for (size_t stateIter = 0; stateIter < mTargetStates.size(); ++stateIter)
		{
			mTargetSM.addTransition(mTargetStates[stateIter], mTransitions[transitionIter]);
		}
	}
}

void GHTransitionChangeTransition::deactivate(void)
{
	mIsActive = false;
	for (size_t transitionIter = 0; transitionIter < mTransitions.size(); ++transitionIter)
	{
		for (size_t stateIter = 0; stateIter < mTargetStates.size(); ++stateIter)
		{
			mTargetSM.removeTransition(mTargetStates[stateIter], mTransitions[transitionIter]);
		}
	}
}

void GHTransitionChangeTransition::addTargetState(const GHIdentifier& targetState)
{
	mTargetStates.push_back(targetState);
}

void GHTransitionChangeTransition::addTransition(GHTransition* transition)
{
	mTransitions.push_back(transition);
}

