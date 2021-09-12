// Copyright 2010 Golden Hammer Software
#include "GHBBGameServices.h"
#include "GHAppShard.h"
#include "GHBBScoreloopScoresFillerXMLLoader.h"
#include "GHAchievementsGUIFillerXMLLoader.h"
#include "GHBBScoreloopOurScoresFillerXMLLoader.h"

GHBBGameServices::GHBBGameServices(GHSystemServices& systemServices,
									GHRenderServices& renderServices,
									GHMessageHandler& appMessageQueue)
: GHGameServices(systemServices, renderServices, appMessageQueue)
{
}

void GHBBGameServices::initAppShard(GHAppShard& appShard,
									const GHTimeVal& time,
									GHRenderGroup& renderGroup,
									GHPhysicsSim& physicsSim)
{
	GHGameServices::initAppShard(appShard, time, renderGroup, physicsSim);

	GHGUITextXMLLoader* guiTextLoader = (GHGUITextXMLLoader*)appShard.mXMLObjFactory.getLoader("guiText");

	if (guiTextLoader)
	{
		GHBBScoreloopOurScoresFillerXMLLoader* ourScoresFiller = new GHBBScoreloopOurScoresFillerXMLLoader(*guiTextLoader, mScoreloopScoresData);
		appShard.mXMLObjFactory.addLoader(ourScoresFiller, 1, "scoreloopOurScoresFiller");


		GHGUIPanelXMLLoader* guiPanelLoader = (GHGUIPanelXMLLoader*)appShard.mXMLObjFactory.getLoader("guiPanel");
		if (guiPanelLoader)
		{
			GHBBScoreloopScoresFillerXMLLoader* scoreloopScoresFiller = new GHBBScoreloopScoresFillerXMLLoader(*guiPanelLoader, *guiTextLoader, mScoreloopScoresData);
			appShard.mXMLObjFactory.addLoader(scoreloopScoresFiller, 1, "scoreloopScoresFiller");

			GHAchievementsGUIFillerXMLLoader* achievementsFiller = new GHAchievementsGUIFillerXMLLoader(*guiPanelLoader, *guiTextLoader, mAchievementsData);
			appShard.mXMLObjFactory.addLoader(achievementsFiller, 1, "achievementsGUIFiller");
		}
	}



}
