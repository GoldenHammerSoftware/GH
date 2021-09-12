// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHThreadSleeper.h"

class GHiOSThreadSleeper : public GHThreadSleeper
{
public:
	virtual void sleep(int ms);
};
