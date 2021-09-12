// Copyright Golden Hammer Software
#pragma once

// base class for anything that wants to update.
class GHController
{
public:
	virtual ~GHController(void) {}
	
	/// perform some arbitrary update action.
	/// this function should never need any arguments, make the class reference the data it needs.
	virtual void update(void) = 0;
	// these are generally called by the controller manager
	// react to being added to an update list.
	virtual void onActivate(void) = 0;
	// react to being removed from an update list.
	virtual void onDeactivate(void) = 0;
};
