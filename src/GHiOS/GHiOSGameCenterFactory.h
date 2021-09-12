// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterFactory.h"
#import <UIKit/UIKit.h>

class GHiOSGameCenterFactory : public GHGameCenterFactory
{
public:
    GHiOSGameCenterFactory(const GHThreadFactory& threadFactory, UIViewController* viewController);
    
    virtual GHGameCenterStatTrackerUIPresenter* createStatTrackerPresenter(void);
    virtual GHGameCenterMultiplayerUIPresenter* createMultiplayerPresenter(void);
    virtual bool isGameCenterAPIAvailable(void);
    
protected:
    virtual GHRefCountedType<GHGameCenterLocalPlayer>* getNewLocalPlayer(GHPropertyContainer& props);

private:
    UIViewController* mViewController;
};
