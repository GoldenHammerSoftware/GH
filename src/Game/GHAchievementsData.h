// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHString/GHString.h"

//A data structure containing the list of achievements.
// Used to pass information from the achievements implementation
// to the GUI for display.
class GHAchievementsData
{
public:
    
	struct Achievement
	{
		GHString mID;
		GHString mName;
		GHString mDescription;
		bool mIsAchieved;
	};
	typedef std::vector<Achievement> AchievementList;
    
	class Callback
	{
	public:
		virtual ~Callback(void) { }
		virtual void onScoresChanged(void) = 0;
	};
    
	void clearData();
	void addAchievement(const Achievement& achievement);
	bool setAchieved(const char* id);
	void registerCallback(Callback* callback);
	void unregisterCallback(Callback* callback);
	void notifyOfChange();
    
	const AchievementList& getAchievements(void) const { return mAchievements; }
    
private:
	AchievementList mAchievements;
	std::vector<Callback*> mCallbacks;
};
