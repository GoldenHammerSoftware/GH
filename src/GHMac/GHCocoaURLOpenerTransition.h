// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTransition.h"
#include "GHString.h"

// a transition that opens a url in the browser.
class GHCocoaURLOpenerTransition : public GHTransition
{
public:
    GHCocoaURLOpenerTransition(const char* url);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    GHString mURLString;
};
