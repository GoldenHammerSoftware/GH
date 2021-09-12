// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHNullController : public GHController
{
public:
	virtual void update(void) { }
	virtual void onActivate(void) { }
	virtual void onDeactivate(void) { }
};
