// Copyright Golden Hammer Software
#pragma once

#include "GHStatTracker.h"

class GHAchievementsData;
class GHXMLSerializer;

// A stat tracker that just stores everything locally without middleware.
// currently supports achievements but not leaderboards.
// We load achievement definitions from achievementcfg.xml
// We save/load achievement progress from achievementSave.exml
class GHStatTrackerLocal : public GHStatTracker
{
public:
	GHStatTrackerLocal(GHAchievementsData& achievementsData, const GHXMLSerializer& xmlSerializer);
	~GHStatTrackerLocal(void);

	virtual void incrementStat(const char* category, int value);
	virtual void setStat(const char* category, int value);
	virtual int getStat(const char* category);
    virtual void incrementScore(const char* category, int value);
    virtual void setScore(const char* category, int value);
    virtual void displayLeaderboards(void);
    
    virtual void setAchievementPrefix(const char* prefix);
    virtual void updateAchievement(const char* achievement, float percentageCompleted);
    virtual void displayAchievements(void);
	virtual const char* getLocalPlayerAlias(void) const;
	virtual uint64_t getLocalPlayerIdentifier(void) const;
	virtual PlayerIDType getPlayerIDType(void) const;
    
private:
	void loadConfig(void);
	void loadProgress(void);
	void saveProgress(void);

private:
    GHAchievementsData& mAchievementsData;
	const GHXMLSerializer& mXMLSerializer;
};
