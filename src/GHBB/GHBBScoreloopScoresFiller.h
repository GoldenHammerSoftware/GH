// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTransition.h"
#include "GHBBScoreloopScoresData.h"
#include "GHGUIWidget.h"

class GHGUIText;

//A transition that fills a GUI Widget with the leaderboard information.
class GHBBScoreloopScoresFiller : public GHTransition
{
public:
	GHBBScoreloopScoresFiller(GHBBScoreloopScoresData& scoresData, GHGUIWidget& mParent);
	~GHBBScoreloopScoresFiller(void);

	void addWidget(GHGUIWidgetResource* widget, GHGUIText* rankText, GHGUIText* nameText, GHGUIText* scoreText);

	virtual void activate(void);
	virtual void deactivate(void);

	void updateGUI(void);

private:
	class Callback : public GHBBScoreloopScoresData::Callback
	{
	public:
		Callback(GHBBScoreloopScoresFiller& filler) : mFiller(filler) { }
		virtual void onScoresChanged(void) { mFiller.updateGUI(); }

	private:
		GHBBScoreloopScoresFiller& mFiller;
	};
	Callback mCallback;

private:
	GHBBScoreloopScoresData& mScoresData;
	GHGUIWidget& mParent;

	struct Widget
	{
		Widget(GHGUIWidgetResource* widget, GHGUIText* rankText, GHGUIText* nameText, GHGUIText* scoreText)
		: mWidget(widget), mRankText(rankText), mNameText(nameText), mScoreText(scoreText), mIsDisplayed(false)
		{ }
		GHGUIWidgetResource* mWidget;
		GHGUIText* mRankText;
		GHGUIText* mNameText;
		GHGUIText* mScoreText;
		bool mIsDisplayed;
	};
	std::vector<Widget> mWidgets;
};
