// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHStatTracker.h"
#include "GHGameCenter.h"
#include "GHString/GHString.h"
#include "GHPlatform/GHRefCounted.h"
#include "GHUpdatedScoreCache.h"
#include "GHUtils/GHController.h"
#include <map>

class GHEventMgr;
class GHGameCenterStatTrackerInterruptHandler;
class GHGameCenterLocalPlayer;
class GHControllerMgr;
class GHStringIdFactory;

class GHGameCenterStatTracker : public GHStatTracker
{
public:
    GHGameCenterStatTracker(GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer,
                            const GHXMLSerializer& xmlSerializer,
                            const GHThreadFactory& threadFactory,
                            GHAchievementNotifier* notifier,
                            GHGameCenterStatTrackerUIPresenter* uiPresenter,
                            GHControllerMgr& mainControllerMgr,
                            GHControllerMgr& osControllerMgr,
                            const GHStringIdFactory& hasher);
    ~GHGameCenterStatTracker(void);

    virtual void incrementScore(const char* category, int value);
    virtual void setScore(const char* category, int value);
    virtual void displayLeaderboards(void);  

    virtual void updateAchievement(const char* achievement, float percentageCompleted);
    virtual void displayAchievements(void);

	virtual void handleEndPauseInterrupt(void) {}
    virtual void osFrameUpdate(void);
    
    void cacheScore(const char* category, int value);
    void cacheScoreIncrement(const char* category, int value);

    virtual void incrementStat(const char* category, int value) {} //todo
    virtual void setStat(const char* category, int value) {} //todo
    virtual int getStat(const char* category) { return 0; } //todo
    virtual const char* getLocalPlayerAlias(void) const { return 0; } //todo
    virtual uint64_t getLocalPlayerIdentifier(void) const { return 0; } //todo
    virtual PlayerIDType getPlayerIDType(void) const { return PID_NONE; } //todo

private:
    void loadAchievementMap(const GHXMLSerializer& serializer);
    
private:
    class MainFrameController : public GHController
    {
    public:
        MainFrameController(GHGameCenter& gameCenter);
        
        virtual void onActivate(void) {}
        virtual void onDeactivate(void) {}
        virtual void update(void);
        
    private:
        GHGameCenter& mGameCenter;
    };
    
    class OSFrameController : public GHController
    {
    public:
        OSFrameController(GHGameCenterStatTracker& parent)
        : mParent(parent) {}

        virtual void onActivate(void) {}
        virtual void onDeactivate(void) {}
        virtual void update(void) { mParent.osFrameUpdate(); }
        
    private:
        GHGameCenterStatTracker& mParent;
    };
    
private:
    GHControllerMgr& mMainControllerMgr;
    GHControllerMgr& mOSControllerMgr;
    
    GHRefCountedType<GHGameCenterLocalPlayer>* mLocalPlayer;
    GHUpdatedScoreCache mScoreCache;
    GHGameCenter mGameCenter;
    bool mAchievementsEnabled;
    bool mAuthCalled;
    // calls update on game center.
    // must be initialized after mGameCenter.
    MainFrameController mMainFrameController;
    OSFrameController mOSFrameController;
    
    typedef std::map<GHString, GHString> StringMap;
    StringMap mGHToGKAchievementMap;
};
