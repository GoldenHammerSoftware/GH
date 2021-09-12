// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHMessageHandler;

// A transition that shows a keyboard on activate and hides it on deactivate
class GHiOSKeyboardTransition : public GHTransition
{
public:
    GHiOSKeyboardTransition(GHMessageHandler& uiMessageQueue);
    ~GHiOSKeyboardTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);

private:
    GHMessageHandler& mUIMessageQueue;
};
