// Copyright 2010 Golden Hammer Software
#include "GHGameCenterFactory.h"
#include "GHGameCenterLocalPlayer.h"
#include "GHGameCenterAuthenticator.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHPropertyContainer.h"
//#import <UIKit/UIKit.h>

GHGameCenterFactory::GHGameCenterFactory(const GHThreadFactory& threadFactory)
: mLocalPlayer(0)
, mThreadFactory(threadFactory)
, mGameCenterNotSupported(false)
{
    
}

GHGameCenterFactory::~GHGameCenterFactory(void)
{

}

GHRefCountedType<GHGameCenterLocalPlayer>* GHGameCenterFactory::getLocalPlayer(GHControllerMgr& osControllerMgr,
                                                                               GHEventMgr& eventMgr, 
                                                                               GHPropertyContainer& props)
{
    if (mGameCenterNotSupported || !isGameCenterAPIAvailable())
    {
        mLocalPlayer = 0;
        mGameCenterNotSupported = true;
        return 0;
    }
    
    if (!mLocalPlayer)
    {
        createLocalPlayer(osControllerMgr, eventMgr, props);
    }
    
    return mLocalPlayer;
}

void GHGameCenterFactory::createLocalPlayer(GHControllerMgr& osControllerMgr,
                                            GHEventMgr& eventMgr,
                                            GHPropertyContainer& props)
{
    mLocalPlayer = getNewLocalPlayer(props);
    
    GHGameCenterAuthenticator* authenticator = new GHGameCenterAuthenticator(eventMgr, *mLocalPlayer->get());
    osControllerMgr.add(authenticator);
}

