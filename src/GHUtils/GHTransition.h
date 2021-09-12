// Copyright Golden Hammer Software
#pragma once

class GHTransition
{
public:
	virtual ~GHTransition(void) {}
	
	virtual void activate(void) = 0;
	virtual void deactivate(void) = 0;
};
