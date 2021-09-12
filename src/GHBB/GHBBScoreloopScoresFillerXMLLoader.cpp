// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopScoresFillerXMLLoader.h"
#include "GHXMLNode.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHGUITextXMLLoader.h"
#include "GHBBScoreloopScoresFiller.h"
#include "GHBBScoreloopScoresData.h"
#include "GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHDebugMessage.h"
#include "GHGUIText.h"

GHBBScoreloopScoresFillerXMLLoader::GHBBScoreloopScoresFillerXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
									   	   	   	   	   	   	   	   	   const GHGUITextXMLLoader& textLoader,
									   	   	   	   	   	   	   	   	   GHBBScoreloopScoresData& scoresData)
: mPanelLoader(panelLoader)
, mTextLoader(textLoader)
, mScoresData(scoresData)
{

}

void* GHBBScoreloopScoresFillerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
	if (!parentWidget) {
		GHDebugMessage::outputString("Failed to find parent widget for GHBBScoreloopScoresFiller.");
		return 0;
	}

	const GHXMLNode* textNode = node.getChild("childText", false);
	if (!textNode) {
		GHDebugMessage::outputString("Failed to find \"childText\" node under scoreloopScoresFiller. Aborting");
		return 0;
	}

	const GHXMLNode* scorePanelNode = node.getChild("scorePanel", false);
	if (!scorePanelNode) {
		GHDebugMessage::outputString("Failed to find \"scorePanel\" node under scoreloopScoresFiller. Aborting");
		return 0;
	}

	GHBBScoreloopScoresFiller* ret = new GHBBScoreloopScoresFiller(mScoresData, *parentWidget);

	for (int i = 0; i < GHBBScoreloopScoresData::MaxDisplayableScores; ++i)
	{
		addWidget(*ret, *textNode, *scorePanelNode, extraData);
	}

	return ret;
}

void GHBBScoreloopScoresFillerXMLLoader::addWidget(GHBBScoreloopScoresFiller& ret, const GHXMLNode& textNode, const GHXMLNode& panelNode, GHPropertyContainer& extraData) const
{
	GHGUIWidgetResource* rankTextResource = (GHGUIWidgetResource*)mTextLoader.create(textNode, extraData);
	GHGUIWidgetResource* nameTextResource = (GHGUIWidgetResource*)mTextLoader.create(textNode, extraData);
	GHGUIWidgetResource* scoreTextResource = (GHGUIWidgetResource*)mTextLoader.create(textNode, extraData);

	GHGUIText* rankText = (GHGUIText*)rankTextResource->get();
	GHGUIText* nameText = (GHGUIText*)nameTextResource->get();
	GHGUIText* scoreText = (GHGUIText*)scoreTextResource->get();

	rankText->setAlignment(GHAlign::A_LEFT, GHAlign::A_CENTER);
	nameText->setAlignment(GHAlign::A_CENTER, GHAlign::A_CENTER);
	scoreText->setAlignment(GHAlign::A_RIGHT, GHAlign::A_CENTER);

	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mPanelLoader.create(panelNode, extraData);
	panel->get()->addChild(rankTextResource);
	panel->get()->addChild(nameTextResource);
	panel->get()->addChild(scoreTextResource);

	ret.addWidget(panel, rankText, nameText, scoreText);
}
