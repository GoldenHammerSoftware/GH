// Copyright 2010 Golden Hammer Software
#pragma once

#include <vector>
#include <list>
#include "GHPlatform/GHRefCounted.h"
#include "GHString/GHIdentifier.h"
#include <map>

class GHXMLSerializer;
class GHMutex;
class GHThreadFactory;
class GHAchievementNotifier;
class GHGameCenterLocalPlayer;
class GHGameCenterStatTrackerUIPresenter;
class GHUpdatedScoreCache;
class GHStringIdFactory;

class GHGameCenter
{
public:
    GHGameCenter(const GHXMLSerializer& xmlSerializer,
                 const GHThreadFactory& threadFactory,
                 GHAchievementNotifier* notifier,
                 GHGameCenterStatTrackerUIPresenter* uiPresenter,
                 const GHStringIdFactory& hasher);
    ~GHGameCenter(void);
    
    void displayLeaderboards(void) const;
    
    void submitScoreIncrement(const char* categoryId, int value);
    void submitScore(const char* categoryId, int value);
    void displayAchivements(void) const;
    void updateAchievement(const char* achievement, float percentageComplete);
    
    void osFrameUpdate(void);
    void gameThreadUpdate(void);
    
    void handleAuthenticationHappened(bool success);
    
    void setScoreCache(GHUpdatedScoreCache* scoreCache) { mScoreCache = scoreCache; }
    
public:
    struct ReportedAchievement
    {
        const static int sIdentifierLen = 128;
        const static int sDescriptionLen = 256;
        ReportedAchievement(void) : mPercentageComplete(0) 
        {
            mIdentifier[0] = 0;
            mDescription[0] = 0;
        }
        
        char mIdentifier[sIdentifierLen];
        char mDescription[sDescriptionLen];
        float mPercentageComplete;
    };
    
private:
    void reportAchievementToGameCenter(const char* achievement, float percentageComplete);
    void submitScore(NSString* categoryId, int value);

    void saveAchievements(void);
    void loadAchievements(std::vector<ReportedAchievement>& achievements);

    int incrementHighScore(const char* categoryId, int networkRetrievedScore, int increment);
    
private:
    const GHXMLSerializer& mXMLSerializer;
    const GHStringIdFactory& mHasher;
    GHGameCenterStatTrackerUIPresenter* mUIPresenter;
    GHUpdatedScoreCache* mScoreCache;
    int mEnabledPropIndex;
    GHMutex* mNotificationMutex;
    GHAchievementNotifier* mNotifier;
    // bool to say we made an attempt at loading network achievements.
    bool mAchievementLoadAttempted;
    // bool to say we were successful at loading network achievements.
    bool mAchievementsLoaded;
    // bool to say we loaded our achievement save game.
    bool mSavedAchievementsLoaded;
    // flag to flush score cache in main thread
    bool mFlushScoreCache;
    bool mWasAuthenticated;
    
    mutable bool mShowLeaderboardRequested;
    mutable bool mShowAchievementRequested;
    
    std::vector<ReportedAchievement> mReportedAchievements;
    std::vector<ReportedAchievement> mAchievementsToUpdate;
    std::vector<ReportedAchievement> mAchievementsToNotify;
    std::vector<ReportedAchievement> mAchievementsLoadedFromSave;
    
    //We need to store high scores locally in addition to retrieving them from GameCenter
    // just in case GameCenter returns an out-of-date score. This can happen
    // if we try to update the score too quickly.
    typedef std::map<GHIdentifier, int> HighScoreMap;
    HighScoreMap mHighScoreCache;
};
