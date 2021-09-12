// Copyright 2010 Golden Hammer Software
#include "GHGameCenter.h"
#import <GameKit/GameKit.h>
#include "GHPlatform/GHMutex.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHAchievementNotifier.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"
#include "GHGameCenterLocalPlayer.h"
#include "GHGameCenterLog.h"
#include "GHGameCenterStatTrackerUIPresenter.h"
#include "GHUpdatedScoreCache.h"
#include "GHString/GHStringIdFactory.h"
#include <algorithm>

GHGameCenter::GHGameCenter(const GHXMLSerializer& xmlSerializer,
                           const GHThreadFactory& threadFactory, 
                           GHAchievementNotifier* notifier,
                           GHGameCenterStatTrackerUIPresenter* uiPresenter,
                           const GHStringIdFactory& hasher)
: mXMLSerializer(xmlSerializer)
, mHasher(hasher)
, mShowLeaderboardRequested(false)
, mShowAchievementRequested(false)
, mAchievementLoadAttempted(false)
, mAchievementsLoaded(false)
, mSavedAchievementsLoaded(false)
, mNotificationMutex(threadFactory.createMutex())
, mNotifier(notifier)
, mUIPresenter(uiPresenter)
, mScoreCache(0)
, mFlushScoreCache(false)
, mWasAuthenticated(false)
{
    mAchievementsToNotify.reserve(100);
}

GHGameCenter::~GHGameCenter(void)
{
    delete mNotificationMutex;
    delete mUIPresenter;
}

static GHGameCenter::ReportedAchievement* findStoredAchievement(const char* achievement,
                                                                 std::vector<GHGameCenter::ReportedAchievement>& list)
{
    std::vector<GHGameCenter::ReportedAchievement>::iterator achIter;
    for (achIter = list.begin(); achIter != list.end(); ++achIter)
    {
        if (!::strcmp(achIter->mIdentifier, achievement))
        {
            return &(*achIter);
        }
    }
    return 0;
}

static void storeNewAchievement(const char* achievement, float percent, const char* description,
                                std::vector<GHGameCenter::ReportedAchievement>& list)
{
    GHGameCenter::ReportedAchievement* re = findStoredAchievement(achievement, list);
    if (re && re->mPercentageComplete >= percent) return;
    if (!re)
    {
        list.push_back(GHGameCenter::ReportedAchievement());
        re = &(list.back());
    }
    ::snprintf(re->mIdentifier, GHGameCenter::ReportedAchievement::sIdentifierLen, "%s", achievement);
    re->mIdentifier[GHGameCenter::ReportedAchievement::sIdentifierLen - 1] = 0;
    if (description) ::snprintf(re->mDescription, GHGameCenter::ReportedAchievement::sDescriptionLen, "%s", description);
    re->mDescription[GHGameCenter::ReportedAchievement::sDescriptionLen - 1] = 0;
    re->mPercentageComplete = percent;
    
}

static void loadNetworkAchievements(std::vector<GHGameCenter::ReportedAchievement>& reportedAchievements, 
                                    bool& achievementsLoaded, bool& achievementLoadAttempted)
{
    // to reset achievements: uncomment this line, run the game once.
    //[GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error){}]; return;
    
    [GKAchievementDescription loadAchievementDescriptionsWithCompletionHandler:^(NSArray *descriptions, NSError *error) 
    {
        if (error != nil || descriptions == nil)
        {
            GHGameCenterLOG("Error loading achievement descriptions.");
            bool* unConstBool = (bool*)&achievementLoadAttempted;
            *unConstBool = true;
        }
        else
        {
            GHGameCenterLOG("Loaded achievement descriptions:");
            for (GKAchievementDescription* desc in descriptions)
            {
                std::vector<GHGameCenter::ReportedAchievement>* unConstList = (std::vector<GHGameCenter::ReportedAchievement>*)&reportedAchievements;
                storeNewAchievement([desc.identifier UTF8String], 0,
                                    [desc.title UTF8String], *unConstList);
                
                //GHGameCenterLOG("%s %s", [desc.identifier UTF8String], [desc.title UTF8String]);
            }
            
            [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *loaderror)
             {
                 if (loaderror != nil)
                 {
                     GHGameCenterLOG("GameCenter Error retrieving achievements");
                 }
                 else
                 {
                     GHGameCenterLOG("GameCenter got achievement list");
                     for (GKAchievement* achievement in achievements)
                     {
                         std::vector<GHGameCenter::ReportedAchievement>* unConstList = (std::vector<GHGameCenter::ReportedAchievement>*)&reportedAchievements;
                         GHGameCenter::ReportedAchievement* currStore = findStoredAchievement([achievement.identifier UTF8String], *unConstList);
                         
                         if (!currStore) 
                         { 
                             GHGameCenterLOG("Found score with no desc!"); 
                         }
                         else
                         {
                             currStore->mPercentageComplete = achievement.percentComplete;
                         }
                     }
                     bool* unConstLoaded = (bool*)&achievementsLoaded;
                     *unConstLoaded = true;
                 }
                 bool* unConstBool = (bool*)&achievementLoadAttempted;
                 *unConstBool = true;
             }];
        }
    }];
}

void GHGameCenter::submitScoreIncrement(const char* categoryId, int value)
{
    NSArray *arr = [[NSArray alloc] initWithObjects:[GKLocalPlayer localPlayer].playerID, nil];
    GKLeaderboard *board = [[GKLeaderboard alloc] initWithPlayerIDs:arr];
    if (board == nil) {
        GHDebugMessage::outputString("Error submitting score increment. Couldn't create leaderboard for player.");
    }
    else {
        board.timeScope = GKLeaderboardTimeScopeAllTime;
        board.range = NSMakeRange(1, 1);
        board.category = [NSString stringWithUTF8String:categoryId];
        [board loadScoresWithCompletionHandler: ^(NSArray* scores, NSError* error)
         {
             if (error != nil)
             {
                 NSLog(@"Error getting score for %@: %@", board.category, [error description]);
                 if (!scores) mScoreCache->addScoreIncrement([board.category UTF8String], value);
             }
             if (scores)
             {
                 int oldScore = ((GKScore*)[scores objectAtIndex:0]).value;
                 int newScore = incrementHighScore(categoryId, oldScore, value);
                 submitScore(board.category, newScore);
             }
             else if (!error)
             {
                 submitScore(board.category, value);
             }
         }];
    }
}

void GHGameCenter::submitScore(const char* categoryId, int value)
{
    incrementHighScore(categoryId, value, 0);
    submitScore([NSString stringWithUTF8String:categoryId], value);
}

void GHGameCenter::submitScore(NSString* categoryId, int value)
{
    GKScore* score = [[GKScore alloc] initWithCategory:categoryId];
    score.value = value;
    [score reportScoreWithCompletionHandler:^(NSError* error)
     {
        if (error != nil)
        {
            GHGameCenterLOG("GameCenter Error Reporting %s score of %d", categoryId, value);
            if (mScoreCache) {
                mScoreCache->addScoreUpdate([categoryId UTF8String], value);
            }
        }
     }
    ];
}

void GHGameCenter::displayAchivements(void) const
{
    mShowLeaderboardRequested = false;
    mShowAchievementRequested = true;
}

void GHGameCenter::updateAchievement(const char* achievement, float percentageComplete)
{
    if (percentageComplete > 100.0f) percentageComplete = 100.0f;
    
    if (mAchievementsLoaded) 
    {
        ReportedAchievement* curr = findStoredAchievement(achievement, mReportedAchievements);
        if (curr && curr->mPercentageComplete >= ::floor(percentageComplete)) return;
    }

    ReportedAchievement* queued = findStoredAchievement(achievement, mAchievementsToUpdate);
    if (!queued)
    {
        mAchievementsToUpdate.push_back(ReportedAchievement());
        queued = &mAchievementsToUpdate.back();
    }
    ::sprintf(queued->mIdentifier, "%s", achievement);
    if (queued->mPercentageComplete < percentageComplete) queued->mPercentageComplete = percentageComplete;
}

void GHGameCenter::reportAchievementToGameCenter(const char* achievement, float percentageComplete)
{
    ReportedAchievement* curr = findStoredAchievement(achievement, mReportedAchievements);
    if (!curr) {
        storeNewAchievement(achievement, percentageComplete, 0, mReportedAchievements);
        curr = findStoredAchievement(achievement, mReportedAchievements);
    }
    curr->mPercentageComplete = percentageComplete;
    
    GKAchievement* gkAchievement = [[GKAchievement alloc] initWithIdentifier:[NSString stringWithUTF8String:achievement]];
    if (gkAchievement)
    {
        gkAchievement.percentComplete = percentageComplete;
        [gkAchievement reportAchievementWithCompletionHandler:^(NSError* error)
         {
             if (error == nil)
             {
                 GHGameCenterLOG("GameCenter reported achievement %s at %f\%", achievement, percentageComplete);
                 
                 ReportedAchievement* reportedAchievement = findStoredAchievement(achievement, mReportedAchievements);
                 
                 if (reportedAchievement)
                 {
                     reportedAchievement->mPercentageComplete = percentageComplete;
                     if (percentageComplete >= 100)
                     {
                         mNotificationMutex->acquire();
                         storeNewAchievement(achievement, percentageComplete, reportedAchievement->mDescription, mAchievementsToNotify);
                         mNotificationMutex->release();
                     }
                
                 }
                 else 
                 {
                     GHGameCenterLOG("Tried to submit an achievement that does not exist.");
                 }
             }
             else
             {
                 GHGameCenterLOG("GameCenter Error Reporting Achievement %s at %f\%", achievement, percentageComplete);
                 GHGameCenterLog::printError(error);
             }
         }
         ];
    }    
}

void GHGameCenter::osFrameUpdate(void)
{
    if (mShowLeaderboardRequested)
    {
        mShowLeaderboardRequested = false;
        mUIPresenter->launchLeaderboard();
    }
    else if (mShowAchievementRequested)
    {
        mShowAchievementRequested = false;
        mUIPresenter->launchAchievements();
    }
}

void GHGameCenter::gameThreadUpdate(void)
{
    if (mAchievementLoadAttempted)
    {
        bool achievementsChanged = false;
        
        for (size_t i = 0; i < mAchievementsToUpdate.size(); ++i)
        {
            ReportedAchievement& re = mAchievementsToUpdate[i];
            reportAchievementToGameCenter(re.mIdentifier, re.mPercentageComplete);
            achievementsChanged = true;
        }
        mAchievementsToUpdate.resize(0);
        
        if (!mSavedAchievementsLoaded)
        {
            mSavedAchievementsLoaded = true;
            loadAchievements(mAchievementsLoadedFromSave);
            for (int i = 0; i < mAchievementsLoadedFromSave.size(); ++i)
            {
                GHGameCenter::ReportedAchievement* curr = findStoredAchievement(mAchievementsLoadedFromSave[i].mIdentifier,
                                                                                 mReportedAchievements);
                float loadedPctComplete = fmin(100.0f, ::floor(mAchievementsLoadedFromSave[i].mPercentageComplete));
                if (!curr || curr->mPercentageComplete < loadedPctComplete)
                {
                    reportAchievementToGameCenter(mAchievementsLoadedFromSave[i].mIdentifier, 
                                                  mAchievementsLoadedFromSave[i].mPercentageComplete);
                    achievementsChanged = true;
                }
            }
        }
        
        if (achievementsChanged)
        {
            saveAchievements();
        }
    }
 
    mNotificationMutex->acquire();
    if (mAchievementsLoaded)
    {
        for (int i = 0; i < mAchievementsToNotify.size(); ++i)
        {
            // display achievement popup.
            if (mNotifier) mNotifier->handleAchievementComplete(mAchievementsToNotify[i].mDescription);
        }
    }
    mAchievementsToNotify.resize(0);
    mNotificationMutex->release();
    
    
    if (mFlushScoreCache)
    {
        mScoreCache->flushScores();
        mFlushScoreCache = false;
    }
}

void GHGameCenter::displayLeaderboards(void) const
{
    mShowLeaderboardRequested = true;
    mShowAchievementRequested = false;
}

void GHGameCenter::saveAchievements(void)
{
    GHXMLNode saveNode;
    saveNode.setName("s", GHString::CHT_REFERENCE);
    for (int i = 0; i < mReportedAchievements.size(); ++i)
    {
        GHXMLNode* achNode = new GHXMLNode;
        achNode->setName("a", GHString::CHT_REFERENCE);
        saveNode.addChild(achNode);
        
        achNode->setAttribute("name", GHString::CHT_REFERENCE, 
                              mReportedAchievements[i].mIdentifier, GHString::CHT_REFERENCE);
        
        char scoreBuf[64];
        ::sprintf(scoreBuf, "%f", mReportedAchievements[i].mPercentageComplete);
        
        achNode->setAttribute("score", GHString::CHT_REFERENCE,
                              scoreBuf, GHString::CHT_COPY);
    }

    mXMLSerializer.writeXMLFile("achievementsave.xml", saveNode);
}

void GHGameCenter::loadAchievements(std::vector<ReportedAchievement>& achievements)
{
	GHXMLNode* node = mXMLSerializer.loadXMLFile("achievementsave.xml");
	if (!node) {
		return;
	}

    const std::vector<GHXMLNode*>& children = node->getChildren();
    for (int i = 0; i < children.size(); ++i)
    {
        const char* name = children[i]->getAttribute("name");
        float percentComplete = 0;
        children[i]->readAttrFloat("score", percentComplete);

        storeNewAchievement(name, percentComplete, 0, achievements);        
    }
    
    delete node;
}

void GHGameCenter::handleAuthenticationHappened(bool success)
{
    if (success && !mWasAuthenticated)
    {
        loadNetworkAchievements(mReportedAchievements, mAchievementsLoaded, mAchievementLoadAttempted);
        mFlushScoreCache = true;
        mWasAuthenticated = true;
    }
    else 
    {
        mAchievementLoadAttempted = true;
    }
}

int GHGameCenter::incrementHighScore(const char* categoryId, int networkRetrievedScore, int increment)
{
    GHIdentifier hash = mHasher.generateHash(categoryId);
    HighScoreMap::iterator iter = mHighScoreCache.find(hash);
    if (iter != mHighScoreCache.end())
    {
        if (iter->second < networkRetrievedScore)
        {
            iter->second = networkRetrievedScore;
        }
        iter->second += increment;
        
        return iter->second;
    }
    else
    {
        int newScore = networkRetrievedScore + increment;
        mHighScoreCache.insert(HighScoreMap::value_type(hash, newScore));
        return newScore;
    }
}




