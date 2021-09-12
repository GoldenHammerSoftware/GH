// Copyright Golden Hammer Software
#pragma once

#include "GHAchievementNotifier.h"
#include "GHUtils/GHController.h"
#include <list>
#include "GHString/GHString.h"
#include "GHString/GHIdentifier.h"

class GHMessageHandler;
class GHPropertyContainer;
class GHTimeVal;

class GHGUIAchievementNotifier : public GHAchievementNotifier, public GHController
{
public:
    GHGUIAchievementNotifier(GHMessageHandler& messageHandler, 
                             const char* guiFile,
                             GHPropertyContainer& props, 
                             const GHIdentifier& displayTimerProp,
                             const GHIdentifier& displayStringProp,
                             const GHTimeVal& time);
    
    virtual void handleAchievementComplete(const char* achievementDesc);
	virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);

private:
    GHMessageHandler& mMessageHandler;
    GHPropertyContainer& mProps;
    const GHTimeVal& mTime;
    GHIdentifier mDisplayTimerProp;
    GHIdentifier mDisplayStringProp;
    GHString mGUIFile;
    
    std::list<GHString> mAchievementsToDisplay;
    float mDisplayStartTime;
};

