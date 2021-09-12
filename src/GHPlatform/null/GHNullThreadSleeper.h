// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadSleeper.h"

class GHNullThreadSleeper : public GHThreadSleeper
{
public:
	virtual void sleep(int ms) {}
};
