// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopScoresFiller.h"
#include "GHGUIWidget.h"
#include "GHGUIText.h"

GHBBScoreloopScoresFiller::GHBBScoreloopScoresFiller(GHBBScoreloopScoresData& scoresData, GHGUIWidget& parent)
: mCallback(*this)
, mScoresData(scoresData)
, mParent(parent)
{
	scoresData.registerCallback(&mCallback);
}

GHBBScoreloopScoresFiller::~GHBBScoreloopScoresFiller(void)
{
	mScoresData.unregisterCallback(&mCallback);
	size_t numWidgets = mWidgets.size();
	for (size_t i = 0; i < numWidgets; ++i)
	{
		Widget& widget = mWidgets[i];
		widget.mWidget->release();
	}
}

void GHBBScoreloopScoresFiller::addWidget(GHGUIWidgetResource* widget, GHGUIText* rankText, GHGUIText* nameText, GHGUIText* scoreText)
{
	widget->acquire();
	mWidgets.push_back(Widget(widget, rankText, nameText, scoreText));
}

void GHBBScoreloopScoresFiller::activate(void)
{

}

void GHBBScoreloopScoresFiller::deactivate(void)
{

}

void GHBBScoreloopScoresFiller::updateGUI(void)
{
	size_t numWidgets = mWidgets.size();
	const GHBBScoreloopScoresData::ScoreList& scoreList = mScoresData.getScores();
	size_t numScores = scoreList.size();

	const int BUF_SZ = 256;
	char buf[BUF_SZ];

	size_t i = 0;
	for (; i < numScores && i < numWidgets; ++i)
	{
		const GHBBScoreloopScoresData::Score& score = scoreList[i];
		Widget& widget = mWidgets[i];
		if (!widget.mIsDisplayed)
		{
			mParent.addChild(widget.mWidget);
			widget.mIsDisplayed = true;
		}
		snprintf(buf, BUF_SZ, "^999#%d", score.mRank);
		widget.mRankText->setText(buf);
		snprintf(buf, BUF_SZ, "^999%s", score.mUsername.getChars());
		widget.mNameText->setText(buf);
		snprintf(buf, BUF_SZ, "^999%s", score.mScore.getChars());
		widget.mScoreText->setText(buf);
	}
	for(; i < numWidgets; ++i)
	{
		Widget& widget = mWidgets[i];
		if (widget.mIsDisplayed)
		{
			mParent.removeChild(widget.mWidget);
			widget.mIsDisplayed = false;
		}
	}

	if (numScores == 0)
	{
		Widget& firstWidget = mWidgets[0];
		if (!firstWidget.mIsDisplayed)
		{
			mParent.addChild(firstWidget.mWidget);
			firstWidget.mIsDisplayed = true;
		}
		firstWidget.mRankText->setText("");
		firstWidget.mScoreText->setText("");
		firstWidget.mNameText->setText("^999No scores found. Check network connection?");
	}
}


