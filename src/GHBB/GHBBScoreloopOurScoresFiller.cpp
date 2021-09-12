// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopOurScoresFiller.h"
#include "GHGUIWidget.h"
#include "GHGUIText.h"
#include <cstring>

GHBBScoreloopOurScoresFiller::GHBBScoreloopOurScoresFiller(GHBBScoreloopScoresData& scoresData,
														   GHGUIText& rankText,
														   GHGUIText& nameText,
														   GHGUIText& scoreText)
: mCallback(*this)
, mScoresData(scoresData)
, mRankText(rankText)
, mNameText(nameText)
, mScoreText(scoreText)
{
	const char* color = nameText.getText();
	if (std::strlen(color) >= 4
		&& color[0] == '^'
		&& color[1] >= '0' && color[1] <= '9'
		&& color[2] >= '0' && color[2] <= '9'
		&& color[3] >= '0' && color[3] <= '9')
	{
		char buf[5];
		buf[0] = '^';
		buf[1] = color[1];
		buf[2] = color[2];
		buf[3] = color[3];
		buf[4] = 0;
		mTextColor.setConstChars(buf, GHString::CHT_COPY);
	}
	else mTextColor.setConstChars("^999", GHString::CHT_REFERENCE);

	scoresData.registerCallback(&mCallback);
}

GHBBScoreloopOurScoresFiller::~GHBBScoreloopOurScoresFiller(void)
{
	mScoresData.unregisterCallback(&mCallback);
}

void GHBBScoreloopOurScoresFiller::activate(void)
{

}

void GHBBScoreloopOurScoresFiller::deactivate(void)
{

}

void GHBBScoreloopOurScoresFiller::updateGUI(void)
{
	if(!mScoresData.getScores().size()) {
		return;
	}

	const int BUF_SZ = 256;
	char buf[BUF_SZ];
	const GHBBScoreloopScoresData::Score& score = mScoresData.getOurScore();
	if (score.mUsername.getChars())
	{
		snprintf(buf, BUF_SZ, "%s%s", mTextColor.getChars(), score.mUsername.getChars());
		mNameText.setText(buf);
	}
	else
	{
		mNameText.setText("");
	}

	if (score.mRank >= 0)
	{
		snprintf(buf, BUF_SZ, "%s#%d", mTextColor.getChars(), score.mRank);
		mRankText.setText(buf);
	}
	else
	{
		mRankText.setText("");
	}

	if (score.mScore.getChars())
	{
		snprintf(buf, BUF_SZ, "%s%s", mTextColor.getChars(), score.mScore.getChars());
		mScoreText.setText(buf);
	}
	else
	{
		mScoreText.setText("");
	}
}

