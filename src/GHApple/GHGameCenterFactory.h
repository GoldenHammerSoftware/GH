// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"

class GHThreadFactory;
class GHControllerMgr;
class GHEventMgr;
class GHPropertyContainer;
class GHGameCenterLocalPlayer;
class GHGameCenterStatTrackerUIPresenter;
class GHGameCenterMultiplayerUIPresenter;
class GHIdentifier;

//Creates GameCenter things that need to be shared between
// different concepts in GHEngine (stat tracking, multiplayer, etc)
// that are singletons fom the GameCenter perspective.

//A single instance of this factory is shared between the factories
// of the GameCenter implementation of the different GHEngine concepts,
// and returns shared or new instances as makes sense from the GameCenter perspective.
class GHGameCenterFactory
{
public:
    GHGameCenterFactory(const GHThreadFactory& threadFactory);
    virtual ~GHGameCenterFactory(void);
    
    GHRefCountedType<GHGameCenterLocalPlayer>* getLocalPlayer(GHControllerMgr& osControllerMgr, GHEventMgr& eventMgr, GHPropertyContainer& props);
    
    virtual GHGameCenterStatTrackerUIPresenter* createStatTrackerPresenter(void) = 0;
    virtual GHGameCenterMultiplayerUIPresenter* createMultiplayerPresenter(void) = 0;
    virtual bool isGameCenterAPIAvailable(void) = 0;
    
protected:
    virtual GHRefCountedType<GHGameCenterLocalPlayer>* getNewLocalPlayer(GHPropertyContainer& props) = 0;
    
private:
    virtual void createLocalPlayer(GHControllerMgr& osControllerMgr,
                                   GHEventMgr& eventMgr,
                                   GHPropertyContainer& props);
    
private:
    GHRefCountedType<GHGameCenterLocalPlayer>* mLocalPlayer;
    const GHThreadFactory& mThreadFactory;
    bool mGameCenterNotSupported;
};
