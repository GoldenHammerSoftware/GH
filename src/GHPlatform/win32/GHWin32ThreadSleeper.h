// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadSleeper.h"

class GHWin32ThreadSleeper : public GHThreadSleeper
{
public:
	virtual void sleep(int ms);
};
