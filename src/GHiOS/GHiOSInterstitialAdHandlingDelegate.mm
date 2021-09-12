// Copyright 2010 Golden Hammer Software
#include "GHiOSInterstitialAdHandlingDelegate.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"

GHiOSInterstitialAdHandlingDelegate::GHiOSInterstitialAdHandlingDelegate(GHMessageQueue& messagequeue)
: mMessageQueue(messagequeue)
{
    
}

void GHiOSInterstitialAdHandlingDelegate::adIsActive(void* adID)
{
    
}

void GHiOSInterstitialAdHandlingDelegate::adDeactivated(void* adID)
{
    mMessageQueue.handleMessage(GHMessage(GHMessageTypes::INTERSTITIALADFINISHED));
}
