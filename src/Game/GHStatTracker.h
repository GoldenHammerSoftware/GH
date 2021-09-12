// Copyright Golden Hammer Software
#pragma once
#include <stdint.h>

// Interface for something that keeps track of achievements and leaderboards.
class GHStatTracker
{
public:
    virtual ~GHStatTracker(void) {}
    
	//stats are just numbers that we might track, which may or may not be tied to achievements
	virtual void incrementStat(const char* category, int value) = 0;
	virtual void setStat(const char* category, int value) = 0;
	virtual int getStat(const char* category) = 0;

	//scores are things we have leaderboards for
    virtual void incrementScore(const char* category, int value) = 0;
    virtual void setScore(const char* category, int value) = 0;
    virtual void displayLeaderboards(void) = 0;
    
    virtual void updateAchievement(const char* achievement, float percentageCompleted) = 0;
    virtual void displayAchievements(void) = 0;

	virtual const char* getLocalPlayerAlias(void) const = 0;
	virtual uint64_t getLocalPlayerIdentifier(void) const = 0;

	enum PlayerIDType
	{
		PID_NONE,
		PID_STEAM,
		PID_GAMECENTER
	};

	virtual PlayerIDType getPlayerIDType(void) const = 0;
};
