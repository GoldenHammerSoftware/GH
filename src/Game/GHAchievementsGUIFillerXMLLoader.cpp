// Copyright Golden Hammer Software
#include "GHAchievementsGUIFillerXMLLoader.h"
#include "GHXMLNode.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHGUITextXMLLoader.h"
#include "GHAchievementsGUIFiller.h"
#include "GHAchievementsData.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIText.h"
#include <cstring>

GHAchievementsGUIFillerXMLLoader::GHAchievementsGUIFillerXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
                                                                   const GHGUITextXMLLoader& textLoader,
                                                                   GHAchievementsData& achievementsData)
: mPanelLoader(panelLoader)
, mTextLoader(textLoader)
, mAchievementsData(achievementsData)
{
    
}

void* GHAchievementsGUIFillerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
	if (!parentWidget) {
		GHDebugMessage::outputString("Failed to find parent widget for GHBBScoreloopAchievementsFiller.");
		return 0;
	}
    
	const GHXMLNode* textNode = node.getChild("childText", false);
	if (!textNode) {
		GHDebugMessage::outputString("Failed to find \"childText\" node under scoreloopAchievementsFiller. Aborting");
		return 0;
	}
    
	const GHXMLNode* descTextNode = node.getChild("descriptionText", false);
	if (!descTextNode) {
		GHDebugMessage::outputString("Failed to find \"descriptionText\" node under scoreloopAchievementsFiller. Aborting");
		return 0;
	}
    
	const GHXMLNode* achievementPanelNode = node.getChild("achievementPanel", false);
	if (!achievementPanelNode) {
		GHDebugMessage::outputString("Failed to find \"achievementPanel\" node under scoreloopAchievementsFiller. Aborting");
		return 0;
	}
    
	const GHXMLNode* unachievedCoverNode = node.getChild("unachievedCover", false);
	if (!unachievedCoverNode) {
		GHDebugMessage::outputString("Failed to find \"unachievedCover\" node under scoreloopAchievementsFiller. Aborting");
		return 0;
	}
    
	GHAchievementsGUIFiller* ret = new GHAchievementsGUIFiller(mAchievementsData, *parentWidget);
    
	size_t numAchievements = mAchievementsData.getAchievements().size();
	for (size_t i = 0; i < numAchievements; ++i)
	{
		const GHAchievementsData::Achievement& achievement = mAchievementsData.getAchievements()[i];
		addWidget(*ret, *textNode, *descTextNode, *achievementPanelNode, *unachievedCoverNode, achievement, extraData);
	}
    
	ret->updateGUI();
    
	return ret;
}

void GHAchievementsGUIFillerXMLLoader::addWidget(GHAchievementsGUIFiller& ret,
                                                 const GHXMLNode& textNode,
                                                 const GHXMLNode& descTextNode,
                                                 const GHXMLNode& panelNode,
                                                 const GHXMLNode& unachievedCoverNode,
                                                 const GHAchievementsData::Achievement& achievement,
                                                 GHPropertyContainer& extraData) const
{
	GHGUIWidgetResource* nameTextResource = (GHGUIWidgetResource*)mTextLoader.create(textNode, extraData);
	GHGUIWidgetResource* descTextResource = (GHGUIWidgetResource*)mTextLoader.create(descTextNode, extraData);
    
	GHGUIText* nameText = (GHGUIText*)nameTextResource->get();
	GHGUIText* descText = (GHGUIText*)descTextResource->get();
    
	const int BUF_SZ = 256;
	char buf[BUF_SZ];
	snprintf(buf, BUF_SZ, "^999%s", achievement.mName.getChars());
	nameText->setText(buf);
	snprintf(buf, BUF_SZ, "^999%s", achievement.mDescription.getChars());
	descText->setText(buf);
    
	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mPanelLoader.create(panelNode, extraData);
    
	GHGUIWidgetResource* cover = 0;
	if (!achievement.mIsAchieved) {
		cover = (GHGUIWidgetResource*)mPanelLoader.create(unachievedCoverNode, extraData);
		panel->get()->addChild(cover);
	}
    
	panel->get()->addChild(nameTextResource);
	panel->get()->addChild(descTextResource);
    
	ret.addWidget(achievement.mName.getChars(), panel, cover, nameText, descText);
    
	GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
	parentWidget->addChild(panel);
}
