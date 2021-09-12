// Copyright Golden Hammer Software
#include "GHStatTrackerFactoryLocal.h"
#include "GHStatTrackerLocal.h"
#include "GHGUITextXMLLoader.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHAppShard.h"
#include "GHAchievementsGUIFillerXMLLoader.h"
#include "GHStatTrackerIdentifiers.h"

GHStatTrackerFactoryLocal::GHStatTrackerFactoryLocal(const GHXMLSerializer& xmlSerializer)
: mXMLSerializer(xmlSerializer)
{
}

GHStatTracker* GHStatTrackerFactoryLocal::createPlatformStatTracker(GHAppShard& appShard, 
	GHAchievementNotifier* notifier) const
{
	GHStatTrackerLocal* ret = new GHStatTrackerLocal(mAchievementsData, mXMLSerializer);
    
    GHGUITextXMLLoader* guiTextLoader = (GHGUITextXMLLoader*)appShard.mXMLObjFactory.getLoader("guiText");
    GHGUIPanelXMLLoader* guiPanelLoader = (GHGUIPanelXMLLoader*)appShard.mXMLObjFactory.getLoader("guiPanel");
    
	if (guiTextLoader && guiPanelLoader)
	{
        GHAchievementsGUIFillerXMLLoader* achievementsFiller = new GHAchievementsGUIFillerXMLLoader(*guiPanelLoader, *guiTextLoader, mAchievementsData);
        appShard.mXMLObjFactory.addLoader(achievementsFiller, 1, "achievementsGUIFiller");
	}

	appShard.mProps.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSACHIEVEMENTS, true);
    appShard.mProps.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSLEADERBOARDS, false);

    return ret;
}
