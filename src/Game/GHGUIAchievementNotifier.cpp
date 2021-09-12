// Copyright Golden Hammer Software
#include "GHGUIAchievementNotifier.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMessageTypes.h"
#include "GHGUIProperties.h"
#include "GHUtils/GHMessage.h"

GHGUIAchievementNotifier::GHGUIAchievementNotifier(GHMessageHandler& messageHandler, 
                                                   const char* guiFile,
                                                   GHPropertyContainer& props, 
                                                   const GHIdentifier& displayTimerProp,
                                                   const GHIdentifier& displayStringProp,
                                                   const GHTimeVal& time)
: mMessageHandler(messageHandler)
, mProps(props)
, mGUIFile(guiFile)
, mDisplayTimerProp(displayTimerProp)
, mDisplayStringProp(displayStringProp)
, mDisplayStartTime(0)
, mTime(time)
{
    mGUIFile.setConstChars(guiFile, GHString::CHT_COPY);
}

void GHGUIAchievementNotifier::handleAchievementComplete(const char* achievementDesc)
{
    //GHDebugMessage::outputDebugString("handled completed achievement %s", achievementDesc);
    
    mAchievementsToDisplay.push_back(GHString());
    mAchievementsToDisplay.back().setConstChars(achievementDesc, GHString::CHT_COPY);
}

void GHGUIAchievementNotifier::update(void)
{
    if (!mAchievementsToDisplay.size())
    {
        return;
    }
    if (mDisplayStartTime == 0)
    {
        mProps.setProperty(mDisplayStringProp, mAchievementsToDisplay.front().getChars());
        mDisplayStartTime = mTime.getTime();
        
        GHPropertyContainer props;
        props.setProperty(GHGUIProperties::GP_WIDGETFILENAME, mGUIFile.getChars());
        mMessageHandler.handleMessage(GHMessage(GHGUIProperties::GUIPUSH, &props));
    }
    const float kTransitionTime = 0.5f;
    const float kDisplayTime = 2.0f;
    float timeSinceDisplay = mTime.getTime() - mDisplayStartTime;
    
    if (timeSinceDisplay > kTransitionTime*2+kDisplayTime)
    {
        GHPropertyContainer props;
        props.setProperty(GHGUIProperties::GP_WIDGETFILENAME, mGUIFile.getChars());
        mMessageHandler.handleMessage(GHMessage(GHGUIProperties::GUIPOP, &props));
        mAchievementsToDisplay.pop_front();
        mDisplayStartTime = 0;
        mProps.setProperty(mDisplayStringProp, 0);
        return;
    }
    
    float transitionPropVal = 1.0f;
    if (timeSinceDisplay < kTransitionTime)
    {
        transitionPropVal = timeSinceDisplay / kTransitionTime;
    }
    else if (timeSinceDisplay > kTransitionTime+kDisplayTime)
    {
        transitionPropVal = 1.0f - ((timeSinceDisplay - (kTransitionTime+kDisplayTime))/kTransitionTime);
    }
    mProps.setProperty(mDisplayTimerProp, transitionPropVal);
}

void GHGUIAchievementNotifier::onActivate(void)
{
    
}

void GHGUIAchievementNotifier::onDeactivate(void)
{
    
}
