// Copyright Golden Hammer Software
#pragma once

class GHThreadSleeper
{
public:
	virtual ~GHThreadSleeper(void) {}
	virtual void sleep(int ms) = 0;
};
