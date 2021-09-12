// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHString.h"

// opens a url when activated
class GHWin32URLOpenerTransition : public GHTransition
{
public:
	GHWin32URLOpenerTransition(const char* url);

	virtual void activate(void);
	virtual void deactivate(void) {}

private:
	GHString mURL;
};
