// Copyright 2010 Golden Hammer Software
#include "GHGameCenterStatTracker.h"
#include "GHGameCenterLocalPlayer.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHGameCenterStatTracker::GHGameCenterStatTracker(GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer,
                                                 const GHXMLSerializer& xmlSerializer,
                                                 const GHThreadFactory& threadFactory,
                                                 GHAchievementNotifier* notifier,
                                                 GHGameCenterStatTrackerUIPresenter* uiPresenter,
                                                 GHControllerMgr& mainControllerMgr,
                                                 GHControllerMgr& osControllerMgr,
                                                 const GHStringIdFactory& hasher)
: mLocalPlayer(localPlayer)
, mGameCenter(xmlSerializer, threadFactory, notifier, uiPresenter, hasher)
, mScoreCache(mGameCenter, *localPlayer->get(), xmlSerializer, threadFactory)
, mAchievementsEnabled(true)
, mAuthCalled(false)
, mMainFrameController(mGameCenter)
, mOSFrameController(*this)
, mMainControllerMgr(mainControllerMgr)
, mOSControllerMgr(osControllerMgr)
{
    mGameCenter.setScoreCache(&mScoreCache);
    mLocalPlayer->acquire();
    mMainControllerMgr.add(&mMainFrameController);
    mOSControllerMgr.add(&mOSFrameController);
    
    loadAchievementMap(xmlSerializer);
}

GHGameCenterStatTracker::~GHGameCenterStatTracker(void)
{
    mLocalPlayer->release();
    mMainControllerMgr.remove(&mMainFrameController);
    mOSControllerMgr.remove(&mOSFrameController);
}

void GHGameCenterStatTracker::osFrameUpdate(void)
{
    if (!mAuthCalled) {
        mLocalPlayer->get()->authenticate(&mGameCenter);
        mAuthCalled = true;
    }
    mGameCenter.osFrameUpdate();
}

void GHGameCenterStatTracker::incrementScore(const char* category, int value)
{
    if (value == 0)
    {
        return;
    }
    
    if (!mLocalPlayer->get()->isAuthenticated())
    {
        mScoreCache.addScoreIncrement(category, value);
        return;
    }
    
    mScoreCache.flushScores();
    mGameCenter.submitScoreIncrement(category, value);
}

void GHGameCenterStatTracker::setScore(const char* category, int value)
{
    if (!mLocalPlayer->get()->isAuthenticated()) 
    {
        mScoreCache.addScoreUpdate(category, value);
        return;
    }
    
    mScoreCache.flushScores();
    mGameCenter.submitScore(category, value);
}

void GHGameCenterStatTracker::displayLeaderboards(void)
{
    mScoreCache.flushScores();
    mGameCenter.displayLeaderboards();
}

void GHGameCenterStatTracker::updateAchievement(const char* achievement, float percentageCompleted)
{
    if (!mAchievementsEnabled) return;
    GHString ghAchievement(achievement, GHString::CHT_REFERENCE);
    StringMap::iterator iter = mGHToGKAchievementMap.find(ghAchievement);
    if (iter != mGHToGKAchievementMap.end())
    {
        mGameCenter.updateAchievement(iter->second.getChars(), percentageCompleted*100.f);
    }
    else
    {   //The string map is optional. If we aren't in the map, then try the input string directly
        mGameCenter.updateAchievement(achievement, percentageCompleted);
    }
}

void GHGameCenterStatTracker::displayAchievements(void)
{
    mScoreCache.flushScores();
    mGameCenter.displayAchivements();
}


GHGameCenterStatTracker::MainFrameController::MainFrameController(GHGameCenter& gameCenter)
: mGameCenter(gameCenter)
{
}

void GHGameCenterStatTracker::MainFrameController::update(void)
{
    mGameCenter.gameThreadUpdate();
}

void GHGameCenterStatTracker::loadAchievementMap(const GHXMLSerializer& serializer)
{
    GHXMLNode* node = serializer.loadXMLFile("stattrackerconfig.xml");
    if (!node) { return; }
    
    const std::vector<GHXMLNode*>& children = node->getChildren();
    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* child = children[i];
        mGHToGKAchievementMap.insert(StringMap::value_type(GHString(child->getName(), GHString::CHT_COPY), GHString(child->getAttribute("id"), GHString::CHT_COPY)));
    }
    delete node;
}

