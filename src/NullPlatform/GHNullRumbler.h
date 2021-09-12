// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRumbler.h"

class GHNullRumbler : public GHRumbler
{
public:
	virtual void setRumble(int, float, float) { }
};
