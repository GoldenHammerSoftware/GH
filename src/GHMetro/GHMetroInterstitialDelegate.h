// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdHandlingDelegate.h"

class GHMessageHandler;

class GHMetroInterstitialDelegate : public GHAdHandlingDelegate
{
public:
    GHMetroInterstitialDelegate(GHMessageHandler& messagequeue);
    
    virtual void adIsActive(void* adID);
	virtual void adDeactivated(void* adID);
    
private:
    GHMessageHandler& mMessageQueue;
};
