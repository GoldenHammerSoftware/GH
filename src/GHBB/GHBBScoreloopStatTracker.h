// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHStatTracker.h"
#include "GHBBScoreloopEventHandler.h"
#include "GHBBScoreloopUser.h"
#include "GHBBScoreloopHighScores.h"
#include "GHBBScoreloopAchievements.h"
#include "GHBBScoreloopDisplayMessageHandler.h"
#include <scoreloop/scoreloopcore.h>
#include "GHController.h"
#include <map>

class GHMutex;
class GHEventMgr;
class GHBBScoreloopScoresData;
class GHAchievementsData;
class GHControllerMgr;
class GHXMLSerializer;

//Central hub for all things Scoreloop, with interface to the game.
//Specific jobs outsourced to aggregated classes.
class GHBBScoreloopStatTracker : public GHStatTracker
{
public:
	GHBBScoreloopStatTracker(GHMutex* mutex,
							 GHBBEventHandlerList& handlerList,
							 GHEventMgr& eventMgr,
							 GHPropertyContainer& props,
							 SC_InitData_t& initData,
							 SC_Client_h& client,
							 GHBBScoreloopScoresData& scoresData,
							 GHAchievementsData& achievementsData,
							 GHControllerMgr& mainControllerMgr,
							 GHControllerMgr& osControllerMgr,
							 const GHXMLSerializer& xmlSerializer);
	~GHBBScoreloopStatTracker(void);

	virtual void gameFrameUpdate(void);
    virtual void osFrameUpdate(void);

    virtual void incrementScore(const char* category, int value);
    virtual void setScore(const char* category, int value);
    virtual void displayLeaderboards(void);
    
    virtual void setAchievementPrefix(const char* prefix);
    virtual void updateAchievement(const char* achievement, float percentageCompleted);
    virtual void displayAchievements(void);
    
    virtual void enableAchievements(bool enable);

private:
    void loadCategoryMap(const GHXMLSerializer& serializer);
    bool getModeFromCategory(const char* category, int& mode) const;

private:
    class MainFrameController : public GHController
    {
    public:
    	MainFrameController(GHBBScoreloopStatTracker& parent)
        : mParent(parent) {}

        virtual void onActivate(void) {}
        virtual void onDeactivate(void) {}
        virtual void update(void) { mParent.gameFrameUpdate(); }

    private:
        GHBBScoreloopStatTracker& mParent;
    };
    class OSFrameController : public GHController
    {
    public:
        OSFrameController(GHBBScoreloopStatTracker& parent)
        : mParent(parent) {}

        virtual void onActivate(void) {}
        virtual void onDeactivate(void) {}
        virtual void update(void) { mParent.osFrameUpdate(); }

    private:
        GHBBScoreloopStatTracker& mParent;
    };

private:
    GHMutex* mMutex;
    GHBBEventHandlerList& mHandlerList;
    GHBBScoreloopEventHandler mEventHandler;
    GHBBScoreloopUser mUser;
    GHBBScoreloopHighScores mHighScores;
    GHBBScoreloopAchievements mAchievements;
    GHBBScoreloopDisplayMessageHandler mDisplayMessageHandler;
    SC_Client_h mClient;
    std::map<GHString, int> mHighScoresCategoryMap;

    GHControllerMgr& mMainControllerMgr;
    GHControllerMgr& mOSControllerMgr;
    OSFrameController mOSFrameController;
    MainFrameController mMainFrameController;
};
