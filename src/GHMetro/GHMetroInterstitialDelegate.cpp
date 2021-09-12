// Copyright 2010 Golden Hammer Software
#include "GHMetroInterstitialDelegate.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"

GHMetroInterstitialDelegate::GHMetroInterstitialDelegate(GHMessageHandler& messagequeue)
: mMessageQueue(messagequeue)
{
    
}

void GHMetroInterstitialDelegate::adIsActive(void* adID)
{
    
}

void GHMetroInterstitialDelegate::adDeactivated(void* adID)
{
    mMessageQueue.handleMessage(GHMessage(GHMessageTypes::INTERSTITIALADFINISHED));
}
