// Copyright Golden Hammer Software
#include "GHAchievementsGUIFiller.h"
#include "GHGUIWidget.h"
#include "GHGUIText.h"
#include <cstring>

GHAchievementsGUIFiller::GHAchievementsGUIFiller(GHAchievementsData& achievementsData, GHGUIWidget& parent)
: mCallback(*this)
, mAchievementsData(achievementsData)
, mParent(parent)
{
	mAchievementsData.registerCallback(&mCallback);
}

GHAchievementsGUIFiller::~GHAchievementsGUIFiller(void)
{
	mAchievementsData.unregisterCallback(&mCallback);
	size_t numWidgets = mWidgets.size();
	for (size_t i = 0; i < numWidgets; ++i)
	{
		Widget& widget = mWidgets[i];
		widget.mWidget->release();
	}
}

void GHAchievementsGUIFiller::addWidget(const char* name, GHGUIWidgetResource* widget, GHGUIWidgetResource* unachievedCoverWidget, 
										GHGUIText* nameText, GHGUIText* descriptionText)
{
	widget->acquire();
	mWidgets.push_back(Widget(name, widget, unachievedCoverWidget, nameText, descriptionText));
}

void GHAchievementsGUIFiller::activate(void)
{
    
}

void GHAchievementsGUIFiller::deactivate(void)
{
    
}

void GHAchievementsGUIFiller::updateGUI(void)
{
	size_t numWidgets = mWidgets.size();
	const GHAchievementsData::AchievementList& achievementList = mAchievementsData.getAchievements();
	size_t numAchievements = achievementList.size();
    
	for (size_t i = 0; i < numAchievements; ++i)
	{
		const GHAchievementsData::Achievement& achievement = achievementList[i];
		for (size_t j = 0; j < numWidgets; ++j)
		{
			Widget& widget = mWidgets[j];
            
			if (!std::strcmp(achievement.mName.getChars(), widget.mName.getChars()))
			{
				if (achievement.mIsAchieved && widget.mUnachievedCoverWidget != 0)
				{
					widget.mWidget->get()->removeChild(widget.mUnachievedCoverWidget);
					widget.mUnachievedCoverWidget = 0;
				}
			}
		}
	}
}

