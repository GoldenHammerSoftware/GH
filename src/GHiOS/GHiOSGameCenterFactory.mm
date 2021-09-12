// Copyright 2010 Golden Hammer Software
#include "GHiOSGameCenterFactory.h"
#include "GHiOSGameCenterStatTrackerUIPresenter.h"
#include "GHiOSGameCenterMultiplayerUIPresenter.h"
#include "GHiOSGameCenterLocalPlayer.h"

GHiOSGameCenterFactory::GHiOSGameCenterFactory(const GHThreadFactory& threadFactory, UIViewController* viewController)
: GHGameCenterFactory(threadFactory)
, mViewController(viewController)
{

}

GHGameCenterStatTrackerUIPresenter* GHiOSGameCenterFactory::GHiOSGameCenterFactory::createStatTrackerPresenter(void)
{
    return new GHiOSGameCenterStatTrackerUIPresenter(mViewController);
}

GHGameCenterMultiplayerUIPresenter* GHiOSGameCenterFactory::createMultiplayerPresenter(void)
{
    return new GHiOSGameCenterMultiplayerUIPresenter(mViewController);
}

bool GHiOSGameCenterFactory::isGameCenterAPIAvailable(void)
{
    // Check for presence of GKLocalPlayer class.
    BOOL localPlayerClassAvailable = (NSClassFromString(@"GKLocalPlayer")) != nil;
    
    // The device must be running iOS 4.1 or later.
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
    return (localPlayerClassAvailable && osVersionSupported);
}

GHRefCountedType<GHGameCenterLocalPlayer>* GHiOSGameCenterFactory::getNewLocalPlayer(GHPropertyContainer& props)
{
    return new GHRefCountedType<GHGameCenterLocalPlayer>(new GHiOSGameCenterLocalPlayer(props, mViewController));
}

