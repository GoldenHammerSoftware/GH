// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "GHiOSAdStack.h"
#include "GHUtils/GHMessageHandler.h"

class GHiOSAdWrapper;
class GHPropertyContainer;
class GHTimeVal;
class GHIdentifier;
class GHEventMgr;
class GHScreenInfo;

class GHiOSAdHandler : public GHController
{
public:
	GHiOSAdHandler(const GHTimeVal& timeVal,
                   GHPropertyContainer& props,
                   const GHIdentifier& displayingAdsProperty,
                   GHEventMgr& gameThreadEventMgr,
                   const GHScreenInfo& screenInfo);
	virtual ~GHiOSAdHandler(void);
	
	virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
	
	void addWrapper(GHiOSAdWrapper* wrapper);

	void handleWindowSizeChanged(void);
    
private:
	void deactivateLowerPriorityAds(int adID);
	bool enableLowerPriorityAds(void);

    void updateIsPortrait(void);

private:
    class MessageListener : public GHMessageHandler
    {
    public:
        MessageListener(GHiOSAdHandler& parent) : mParent(parent) {}
        virtual void handleMessage(const GHMessage& message);
        
    private:
        GHiOSAdHandler& mParent;
    };
    
private:
    GHiOSAdStack mAdStack;
	const GHTimeVal& mTimeVal;
    GHEventMgr& mGameThreadEventMgr;
    bool mIsPortrait;
    bool mPortraitDirty;
	bool mInitialized;
    MessageListener mMessageListener;
    const GHScreenInfo& mScreenInfo;
};
