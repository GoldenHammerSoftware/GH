// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopScoresData.h"
#include <algorithm>

void GHBBScoreloopScoresData::clearData()
{
	mData.clear();
}

void GHBBScoreloopScoresData::setOurScore(const Score& score)
{
	mOurScore = score;
}

void GHBBScoreloopScoresData::addScore(const Score& score)
{
	mData.push_back(score);
}

void GHBBScoreloopScoresData::registerCallback(Callback* callback)
{
	mCallbacks.push_back(callback);
}

void GHBBScoreloopScoresData::unregisterCallback(Callback* callback)
{
	std::vector<Callback*>::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	if (iter != mCallbacks.end())
	{
		mCallbacks.erase(iter);
	}
}

void GHBBScoreloopScoresData::notifyOfChange()
{
	size_t numCallbacks = mCallbacks.size();
	for (size_t i = 0; i < numCallbacks; ++i) {
		Callback* callback = mCallbacks[i];
		callback->onScoresChanged();
	}
}
