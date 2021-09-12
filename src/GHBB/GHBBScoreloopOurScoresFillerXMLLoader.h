// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUITextXMLLoader;
class GHBBScoreloopScoresData;
class GHBBScoreloopScoresFiller;

//For filling in the text just of the panel that lists
// the current player's score.
class GHBBScoreloopOurScoresFillerXMLLoader : public GHXMLObjLoader
{
public:
	GHBBScoreloopOurScoresFillerXMLLoader(const GHGUITextXMLLoader& textLoader,
										  GHBBScoreloopScoresData& scoresData);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
	const GHGUITextXMLLoader& mTextLoader;
	GHBBScoreloopScoresData& mScoresData;
};
