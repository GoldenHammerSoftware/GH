#pragma once

#include "GHUtils/GHTransition.h"

class GHNullTransition : public GHTransition
{
public:
	virtual void activate(void) { }
	virtual void deactivate(void) { }
};
