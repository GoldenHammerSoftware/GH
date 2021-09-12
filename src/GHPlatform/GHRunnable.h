// Copyright Golden Hammer Software
#pragma once

class GHRunnable;

// Something that can do something on a run call.  Used for making threads execute code.
class GHRunnable
{
public:
    virtual ~GHRunnable(void) { }
	
	virtual void run(void) = 0;
};
