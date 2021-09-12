// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

// causes the app to exit when triggered.
class GHWin32ExitAppTransition : public GHTransition
{
public:
	virtual void activate(void);
	virtual void deactivate(void) {}
};
