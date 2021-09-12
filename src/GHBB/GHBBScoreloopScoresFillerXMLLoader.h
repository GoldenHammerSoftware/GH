// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUIPanelXMLLoader;
class GHGUITextXMLLoader;
class GHBBScoreloopScoresData;
class GHBBScoreloopScoresFiller;

//Loader for a transition that fills the parent GUI with the leaderboard information.
class GHBBScoreloopScoresFillerXMLLoader : public GHXMLObjLoader
{
public:
	GHBBScoreloopScoresFillerXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
									   const GHGUITextXMLLoader& textLoader,
									   GHBBScoreloopScoresData& scoresData);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
    void addWidget(GHBBScoreloopScoresFiller& ret, const GHXMLNode& textNode, const GHXMLNode& panelNode, GHPropertyContainer& extraData) const;

private:
	const GHGUIPanelXMLLoader& mPanelLoader;
	const GHGUITextXMLLoader& mTextLoader;
	GHBBScoreloopScoresData& mScoresData;
};
