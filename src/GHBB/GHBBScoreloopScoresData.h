// Copyright 2010 Golden Hammer Software
#pragma once

#include <vector>
#include "GHString.h"

//A data structure containing the list of scores in the leaderboard.
// Used to pass information from the achievements implementation
// to the GUI for display.
class GHBBScoreloopScoresData
{
public:

	const static int MaxDisplayableScores = 25;

	struct Score
	{
		GHString mUsername;
		GHString mScore;
		int mRank;

		Score(void)
		: mRank(-1)
		{}

		Score& operator=(const Score& other)
		{
			mUsername.setConstChars(other.mUsername.getChars(), GHString::CHT_COPY);
			mScore.setConstChars(other.mScore.getChars(), GHString::CHT_COPY);
			mRank = other.mRank;
			return *this;
		}
	};
	typedef std::vector<Score> ScoreList;

	class Callback
	{
	public:
		virtual ~Callback(void) { }
		virtual void onScoresChanged(void) = 0;
	};

	void clearData();
	void setOurScore(const Score& score);
	void addScore(const Score& score);
	void registerCallback(Callback* callback);
	void unregisterCallback(Callback* callback);
	void notifyOfChange();

	const ScoreList& getScores(void) const { return mData; }
	const Score& getOurScore(void) const { return mOurScore; }

private:
	ScoreList mData;
	std::vector<Callback*> mCallbacks;
	Score mOurScore;
};
