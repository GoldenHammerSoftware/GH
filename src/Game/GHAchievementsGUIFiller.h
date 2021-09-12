// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHAchievementsData.h"
#include "GHGUIWidget.h"
#include "GHString/GHString.h"

class GHGUIText;

//A transition that fills a GUI Widget with information
// about achievements.
class GHAchievementsGUIFiller : public GHTransition
{
public:
	GHAchievementsGUIFiller(GHAchievementsData& scoresData, GHGUIWidget& mParent);
	~GHAchievementsGUIFiller(void);
    
	void addWidget(const char* name, GHGUIWidgetResource* widget, GHGUIWidgetResource* unachievedCoverWidget, GHGUIText* nameText, GHGUIText* descriptionText);
    
	virtual void activate(void);
	virtual void deactivate(void);
    
	void updateGUI(void);
    
private:
	class Callback : public GHAchievementsData::Callback
	{
	public:
		Callback(GHAchievementsGUIFiller& filler) : mFiller(filler) { }
		virtual void onScoresChanged(void) { mFiller.updateGUI(); }
        
	private:
		GHAchievementsGUIFiller& mFiller;
	};
	Callback mCallback;
    
private:
	GHAchievementsData& mAchievementsData;
	GHGUIWidget& mParent;
    
	struct Widget
	{
		Widget(const char* name, GHGUIWidgetResource* widget, GHGUIWidgetResource* unachievedCoverWidget, GHGUIText* nameText, GHGUIText* descriptionText)
		: mWidget(widget), mUnachievedCoverWidget(unachievedCoverWidget), mNameText(nameText), mDescriptionText(descriptionText)
		{ mName.setConstChars(name, GHString::CHT_COPY); }
		GHGUIWidgetResource* mWidget;
		GHGUIWidgetResource* mUnachievedCoverWidget;
		GHGUIText* mNameText;
		GHGUIText* mDescriptionText;
		GHString mName;
	};
	std::vector<Widget> mWidgets;
};
