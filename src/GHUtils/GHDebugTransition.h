// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHString.h"

//A class for debugging when stuff happens, mainly in the gui.
// Use case: put it in the gui's transition in or out to determine when it happens.
//  or put it in any other state machine to determine when the state is getting activated/deactivated
class GHDebugTransition : public GHTransition
{
public:
	GHDebugTransition(const char* identifier);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHString mIdentifier;
};
