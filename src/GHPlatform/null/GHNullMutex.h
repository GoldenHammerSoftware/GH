// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHMutex.h"

class GHNullMutex : public GHMutex
{
public:
	virtual bool acquire(void) { return true; }
	virtual bool release(void) { return true; }
};
