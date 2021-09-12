// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTransition.h"
#include "GHBBScoreloopScoresData.h"
#include "GHString.h"

class GHGUIText;

//A transition that fills a GUI Widget with information
// about specifically the current user's high score.
class GHBBScoreloopOurScoresFiller : public GHTransition
{
public:
	GHBBScoreloopOurScoresFiller(GHBBScoreloopScoresData& scoresData, GHGUIText& rankText, GHGUIText& nameText, GHGUIText& scoreText);
	~GHBBScoreloopOurScoresFiller(void);

	virtual void activate(void);
	virtual void deactivate(void);

	void updateGUI(void);

private:
	class Callback : public GHBBScoreloopScoresData::Callback
	{
	public:
		Callback(GHBBScoreloopOurScoresFiller& filler) : mFiller(filler) { }
		virtual void onScoresChanged(void) { mFiller.updateGUI(); }

	private:
		GHBBScoreloopOurScoresFiller& mFiller;
	};
	Callback mCallback;

private:
	GHBBScoreloopScoresData& mScoresData;
	GHGUIText& mRankText;
	GHGUIText& mNameText;
	GHGUIText& mScoreText;
	GHString mTextColor;
};
