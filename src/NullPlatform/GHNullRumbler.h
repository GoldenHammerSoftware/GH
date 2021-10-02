// Copyright 2010 Golden Hammer Software
#pragma once

#include "Base/GHRumbler.h"

class GHNullRumbler : public GHRumbler
{
public:
	virtual void setRumble(int, float, float) { }
};
