// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadSignal.h"

class GHNullThreadSignal : public GHThreadSignal
{
public:
	virtual bool wait(GHMutex& mutex) { return false; }
	virtual bool signal() { return false; }
};
