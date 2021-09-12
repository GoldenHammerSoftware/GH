// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"

// a struct to store the arguments required to create a gui transform.
class GHGUITransitionDesc
{
public:
	GHGUITransitionDesc(void) : mTransitionProp(0), mTransitionTime(0), mTransitionWait(0) {}
	GHGUITransitionDesc(const GHIdentifier& prop, float time, float wait)
		: mTransitionProp(prop)
		, mTransitionTime(time)
		, mTransitionWait(wait)
	{}

	// the property we key into to see how far along we are in the transition.
	GHIdentifier mTransitionProp;
	// how long the transition takes.
	float mTransitionTime;
	// how long to wait before triggering the transition.
	float mTransitionWait;
};
