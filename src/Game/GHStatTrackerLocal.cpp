// Copyright Golden Hammer Software
#include "GHStatTrackerLocal.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHAchievementsData.h"

GHStatTrackerLocal::GHStatTrackerLocal(GHAchievementsData& achievementsData, const GHXMLSerializer& xmlSerializer)
: mAchievementsData(achievementsData)
, mXMLSerializer(xmlSerializer)
{
	loadConfig();
	loadProgress();
}

GHStatTrackerLocal::~GHStatTrackerLocal(void)
{
}

void GHStatTrackerLocal::incrementStat(const char* category, int value)
{
	// unsupported
}

void GHStatTrackerLocal::setStat(const char* category, int value)
{
	// unsupported
}

int GHStatTrackerLocal::getStat(const char* category)
{
	// unsuported
	return 0;
}

void GHStatTrackerLocal::incrementScore(const char* category, int value)
{
	// unsupported
}

void GHStatTrackerLocal::setScore(const char* category, int value)
{
	// unsupported
}

void GHStatTrackerLocal::displayLeaderboards(void)
{
	// unsupported
}
    
void GHStatTrackerLocal::setAchievementPrefix(const char* prefix)
{
	// doesn't mean anything here.
}

const char* GHStatTrackerLocal::getLocalPlayerAlias(void) const
{
	//no local player alias
	return 0;
}

uint64_t GHStatTrackerLocal::getLocalPlayerIdentifier(void) const
{
	//no local player identifier
	return 0;
}

GHStatTracker::PlayerIDType GHStatTrackerLocal::getPlayerIDType(void) const
{
	return PID_NONE;
}

void GHStatTrackerLocal::updateAchievement(const char* achievement, float percentageCompleted)
{
	if (!achievement) {
		return;
	}

	if (percentageCompleted > 0)
	{
		if (mAchievementsData.setAchieved(achievement))
		{
			saveProgress();
		}
	}
}

void GHStatTrackerLocal::displayAchievements(void)
{
	// unsupported: we just show a gh gui panel from another gh gui panel
}

void GHStatTrackerLocal::loadConfig(void)
{
	GHXMLNode* configNode = mXMLSerializer.loadXMLFile("achievementcfg.xml");
	if (!configNode) {
		return;
	}

	for (size_t i = 0; i < configNode->getChildren().size(); ++i)
	{
		const GHXMLNode* childNode = configNode->getChildren()[i];
		GHAchievementsData::Achievement achievement;
		achievement.mIsAchieved = false;
		achievement.mID.setConstChars(childNode->getAttribute("id"), GHString::CHT_COPY);
		achievement.mName.setConstChars(childNode->getAttribute("name"), GHString::CHT_COPY);
		achievement.mDescription.setConstChars(childNode->getAttribute("desc"), GHString::CHT_COPY);
		mAchievementsData.addAchievement(achievement);
	}

	delete configNode;
}

const char kEncryptKey = 24;

void GHStatTrackerLocal::loadProgress(void)
{
	GHXMLNode* storeNode = mXMLSerializer.loadEncryptedFile("achievementSave.exml", kEncryptKey);
	if (!storeNode) {
		return;
	}
	for (size_t i = 0; i < storeNode->getChildren().size(); ++i)
	{
		const GHXMLNode* childNode = storeNode->getChildren()[i];
		mAchievementsData.setAchieved(childNode->getName());
	}
	delete storeNode;
}

void GHStatTrackerLocal::saveProgress(void)
{
	GHXMLNode storeNode;
	storeNode.setName("achievements", GHString::CHT_REFERENCE);
	const GHAchievementsData::AchievementList& achievements = mAchievementsData.getAchievements();
	for (size_t i = 0; i < achievements.size(); ++i)
	{
		if (!achievements[i].mIsAchieved) continue;
		GHXMLNode* achSave = new GHXMLNode;
		achSave->setName(achievements[i].mName.getChars(), GHString::CHT_REFERENCE);
		storeNode.addChild(achSave);
	}
	mXMLSerializer.writeEncryptedFile("achievementSave.exml", storeNode, kEncryptKey);
}
