// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterFactory.h"

class GHCocoaGameCenterFactory : public GHGameCenterFactory
{
public:
    GHCocoaGameCenterFactory(const GHThreadFactory& threadFactory, NSWindow* window);
    
    virtual GHGameCenterStatTrackerUIPresenter* createStatTrackerPresenter(void);
    virtual GHGameCenterMultiplayerUIPresenter* createMultiplayerPresenter(void);
    virtual bool isGameCenterAPIAvailable(void);
    
protected:
    virtual GHRefCountedType<GHGameCenterLocalPlayer>* getNewLocalPlayer(GHPropertyContainer& props);

private:
    NSWindow* mWindow;
};
