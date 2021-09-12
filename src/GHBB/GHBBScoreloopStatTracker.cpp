// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopStatTracker.h"
#include "GHControllerMgr.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHBBScoreloopStatTracker::GHBBScoreloopStatTracker(GHMutex* mutex,
												   GHBBEventHandlerList& handlerList,
												   GHEventMgr& eventMgr,
												   GHPropertyContainer& props,
												   SC_InitData_t& initData,
												   SC_Client_h& client,
												   GHBBScoreloopScoresData& scoresData,
												   GHAchievementsData& achievementsData,
												   GHControllerMgr& mainControllerMgr,
												   GHControllerMgr& osControllerMgr,
												   const GHXMLSerializer& xmlSerializer)
: mMutex(mutex)
, mHandlerList(handlerList)
, mEventHandler(initData)
, mUser(client)
, mHighScores(client, mUser, mutex, scoresData, props)
, mAchievements(client, mUser, mutex, achievementsData, props)
, mDisplayMessageHandler(eventMgr, mAchievements, mHighScores, props)
, mClient(client)
, mMainControllerMgr(mainControllerMgr)
, mOSFrameController(*this)
, mMainFrameController(*this)
, mOSControllerMgr(osControllerMgr)
{
	mHandlerList.push_back(&mEventHandler);
    mMainControllerMgr.add(&mMainFrameController);
    mOSControllerMgr.add(&mOSFrameController);

    loadCategoryMap(xmlSerializer);
}

GHBBScoreloopStatTracker::~GHBBScoreloopStatTracker(void)
{
	GHBBEventHandlerList::iterator iter = std::find(mHandlerList.begin(), mHandlerList.end(), &mEventHandler);
	if (iter != mHandlerList.end())
	{
		mHandlerList.erase(iter);
	}

	SC_Client_Release(mClient);

    mMainControllerMgr.remove(&mMainFrameController);
    mOSControllerMgr.remove(&mOSFrameController);
}

void GHBBScoreloopStatTracker::gameFrameUpdate(void)
{
	mHighScores.gameFrameUpdate();
	mAchievements.gameFrameUpdate();
}

void GHBBScoreloopStatTracker::osFrameUpdate(void)
{
	mHighScores.osFrameUpdate();
	mAchievements.osFrameUpdate();
}

void GHBBScoreloopStatTracker::incrementScore(const char* category, int value)
{
	int mode;
	if (getModeFromCategory(category, mode)) {
		mHighScores.incrementScore(mode, value);
	}
}

void GHBBScoreloopStatTracker::setScore(const char* category, int value)
{
	int mode;
	if (getModeFromCategory(category, mode)) {
		mHighScores.submitScore(mode, value);
	}
}

void GHBBScoreloopStatTracker::displayLeaderboards(void)
{
	//This gets called through a message, handled by GHBBScoreloopDisplayMessageHandler
	//mHighScores.fetchLeaderboard();
}

void GHBBScoreloopStatTracker::setAchievementPrefix(const char* prefix)
{
}

void GHBBScoreloopStatTracker::updateAchievement(const char* achievement, float percentageCompleted)
{
	mAchievements.updateAchievement(achievement, percentageCompleted);
}

void GHBBScoreloopStatTracker::displayAchievements(void)
{
	//This gets called through a message, handled by GHBBScoreloopDisplayMessageHandler
	//mAchievements.fetchAchievements();
}

void GHBBScoreloopStatTracker::enableAchievements(bool enable)
{
}

void GHBBScoreloopStatTracker::loadCategoryMap(const GHXMLSerializer& serializer)
{
	GHXMLNode* file = serializer.loadXMLFile("scoreloopscoresconfig.xml");

	if (!file) {
		GHDebugMessage::outputString("Failed to load scoreloopscoresconfig.xml. You will not be able to submit any high scores.");
		return;
	}

	const GHXMLNode* categoriesNode = file->getChild("categories", false);
	if (categoriesNode)
	{
		const std::vector<GHXMLNode*>& children = categoriesNode->getChildren();
		size_t numChildren = children.size();
		for (size_t i = 0; i < numChildren; ++i)
		{
			const GHXMLNode* child = children[i];
			const char* name = child->getAttribute("name");
			int val;
			bool valRead = child->readAttrInt("val", val);
			if (name && valRead)
			{
				GHString ghString(name, GHString::CHT_COPY);
				mHighScoresCategoryMap[ghString] = val;
			}
			else
			{
				GHDebugMessage::outputString("Error reading a scoreloop high scores config node.");
			}
		}
	}
	else
	{
		GHDebugMessage::outputString("Failed to find \"categories\" node in scoreloopscoresconfig.xml. You will not be able to submit any high scores.");
	}

	delete file;
}

bool GHBBScoreloopStatTracker::getModeFromCategory(const char* category, int& mode) const
{
	GHString findStr(category, GHString::CHT_REFERENCE);
	std::map<GHString, int>::const_iterator iter = mHighScoresCategoryMap.find(findStr);
	if (iter == mHighScoresCategoryMap.end())
	{
		GHDebugMessage::outputString("Error finding category %s in the scoreloop high scores category map. The score will not be updated or retrieved.", category);
		return false;
	}
	mode = iter->second;
	return true;
}
