// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopOurScoresFillerXMLLoader.h"
#include "GHXMLNode.h"
#include "GHGUITextXMLLoader.h"
#include "GHBBScoreloopOurScoresFiller.h"
#include "GHBBScoreloopScoresData.h"
#include "GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHDebugMessage.h"
#include "GHGUIText.h"

GHBBScoreloopOurScoresFillerXMLLoader::GHBBScoreloopOurScoresFillerXMLLoader(const GHGUITextXMLLoader& textLoader,
									   	   	   	   	   	   	   	   	   	     GHBBScoreloopScoresData& scoresData)
: mTextLoader(textLoader)
, mScoresData(scoresData)
{

}

void* GHBBScoreloopOurScoresFillerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
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

	GHGUIWidgetResource* rankTextResource = (GHGUIWidgetResource*)mTextLoader.create(*textNode, extraData);
	GHGUIWidgetResource* nameTextResource = (GHGUIWidgetResource*)mTextLoader.create(*textNode, extraData);
	GHGUIWidgetResource* scoreTextResource = (GHGUIWidgetResource*)mTextLoader.create(*textNode, extraData);

	GHGUIText* rankText = (GHGUIText*)rankTextResource->get();
	GHGUIText* nameText = (GHGUIText*)nameTextResource->get();
	GHGUIText* scoreText = (GHGUIText*)scoreTextResource->get();

	rankText->setAlignment(GHAlign::A_LEFT, GHAlign::A_CENTER);
	nameText->setAlignment(GHAlign::A_CENTER, GHAlign::A_CENTER);
	scoreText->setAlignment(GHAlign::A_RIGHT, GHAlign::A_CENTER);

	parentWidget->addChild(rankTextResource);
	parentWidget->addChild(nameTextResource);
	parentWidget->addChild(scoreTextResource);

	GHBBScoreloopOurScoresFiller* ret = new GHBBScoreloopOurScoresFiller(mScoresData, *rankText, *nameText, *scoreText);
	return ret;
}

