// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadSleeper.h"

class GHMetroThreadSleeper : public GHThreadSleeper
{
public:
	virtual void sleep(int ms);
};
