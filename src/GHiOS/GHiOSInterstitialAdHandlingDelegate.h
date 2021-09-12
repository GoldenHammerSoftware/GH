// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdHandlingDelegate.h"

class GHMessageQueue;

class GHiOSInterstitialAdHandlingDelegate : public GHAdHandlingDelegate
{
public:
    GHiOSInterstitialAdHandlingDelegate(GHMessageQueue& messagequeue);
    
    virtual void adIsActive(void* adID);
	virtual void adDeactivated(void* adID);
    
private:
    GHMessageQueue& mMessageQueue;
};
