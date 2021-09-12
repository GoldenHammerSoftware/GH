// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHString.h"

class GHiOSURLOpenerTransiton : public GHTransition
{
public:
    GHiOSURLOpenerTransiton(const char* url);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    GHString mURLString;
};

