// Copyright Golden Hammer Software
#pragma once

#include "GHStatTracker.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHStatTrackerIdentifiers.h"

class GHNullStatTracker : public GHStatTracker
{
public:
    GHNullStatTracker(GHPropertyContainer& props)
    {
        props.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSACHIEVEMENTS, false);
        props.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSLEADERBOARDS, false);
    }
    
	virtual void incrementStat(const char* category, int value) { }
	virtual void setStat(const char* category, int value) { }
	virtual int getStat(const char* category) { return 0; }

    virtual void incrementScore(const char* category, int value) { }
    virtual void setScore(const char* , int ) { }
    virtual void displayLeaderboards(void) { }

    virtual void setAchievementPrefix(const char* prefix) { }
    virtual void updateAchievement(const char*, float ) { }
    virtual void displayAchievements(void) { }

	virtual const char* getLocalPlayerAlias(void) const { return  ""; }
	virtual uint64_t getLocalPlayerIdentifier(void) const { return 0; }
	virtual PlayerIDType getPlayerIDType(void) const { return PID_NONE; }
};

