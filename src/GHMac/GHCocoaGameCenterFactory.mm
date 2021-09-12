// Copyright 2010 Golden Hammer Software
#include "GHCocoaGameCenterFactory.h"
#include "GHCocoaGameCenterMultiplayerUIPresenter.h"
#include "GHCocoaGameCenterStatTrackerUIPresenter.h"
#include "GHCocoaGameCenterLocalPlayer.h"

GHCocoaGameCenterFactory::GHCocoaGameCenterFactory(const GHThreadFactory& threadFactory, NSWindow* window)
: GHGameCenterFactory(threadFactory)
, mWindow(window)
{
    
}

GHGameCenterStatTrackerUIPresenter* GHCocoaGameCenterFactory::createStatTrackerPresenter(void)
{
    return new GHCocoaGameCenterStatTrackerUIPresenter(mWindow);
}

GHGameCenterMultiplayerUIPresenter* GHCocoaGameCenterFactory::createMultiplayerPresenter(void)
{
    return new GHCocoaGameCenterMultiplayerUIPresenter(mWindow);
}

bool GHCocoaGameCenterFactory::isGameCenterAPIAvailable(void)
{
    return true;
}

GHRefCountedType<GHGameCenterLocalPlayer>* GHCocoaGameCenterFactory::getNewLocalPlayer(GHPropertyContainer& props)
{
    return new GHRefCountedType<GHGameCenterLocalPlayer>(new GHCocoaGameCenterLocalPlayer(props));
}
