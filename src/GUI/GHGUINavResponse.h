// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHGUINavResponse
{
public:
	virtual ~GHGUINavResponse(void) { }

	virtual bool respondToNavigation(const GHPoint2i& navDir) = 0;
};
