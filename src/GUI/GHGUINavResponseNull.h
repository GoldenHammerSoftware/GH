// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGUINavResponse.h"

class GHGUINavResponseNull : public GHGUINavResponse
{
public:
	virtual bool respondToNavigation(const GHPoint2i& navDir)
	{
		return true;
	}
};
