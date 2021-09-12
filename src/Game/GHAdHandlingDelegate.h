// Copyright Golden Hammer Software
#pragma once

// An interface for ads to tell somebody when they are visible/dismissed.
class GHAdHandlingDelegate
{
public:
    virtual ~GHAdHandlingDelegate(void) { }
	
	virtual void adIsActive(void* adID) = 0;
	virtual void adDeactivated(void* adID) = 0;
};
