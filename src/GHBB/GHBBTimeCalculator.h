// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTimeCalculator.h"
#include <sys/time.h>

class GHBBTimeCalculator : public GHTimeCalculator
{
public:
	GHBBTimeCalculator(void);
	virtual float calcTime(void) const;

protected:
	struct timeval mStartupTime;
};
