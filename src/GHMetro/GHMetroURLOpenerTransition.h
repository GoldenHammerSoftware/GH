// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

// A transition to open a url when triggered.
class GHMetroURLOpenerTransition : public GHTransition
{
public:
	GHMetroURLOpenerTransition(const char* url);

	virtual void activate(void);
	virtual void deactivate(void) {}

private:
	Platform::String^ mURLString;
};
