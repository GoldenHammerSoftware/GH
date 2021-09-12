// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMutex.h"

class GHCocoaMutex : public GHMutex
{
public:
	GHCocoaMutex(void);
	~GHCocoaMutex(void);
	
	virtual bool acquire(void);
	virtual bool release(void);
	
private:
	NSRecursiveLock* mLock;
};
