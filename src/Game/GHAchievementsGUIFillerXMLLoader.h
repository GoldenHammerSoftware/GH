// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHAchievementsData.h"

class GHGUIPanelXMLLoader;
class GHGUITextXMLLoader;
class GHAchievementsGUIFiller;

//A loader for a transition that fills the parent GUI with information
// about achievements.
class GHAchievementsGUIFillerXMLLoader : public GHXMLObjLoader
{
public:
	GHAchievementsGUIFillerXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
                                     const GHGUITextXMLLoader& textLoader,
                                     GHAchievementsData& scoresData);
    
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void addWidget(GHAchievementsGUIFiller& ret,
    			   const GHXMLNode& textNode,
    			   const GHXMLNode& descTextNode,
    			   const GHXMLNode& panelNode,
    			   const GHXMLNode& unachievedCoverNode,
    			   const GHAchievementsData::Achievement& achievement,
    			   GHPropertyContainer& extraData) const;
    
private:
	const GHGUIPanelXMLLoader& mPanelLoader;
	const GHGUITextXMLLoader& mTextLoader;
	GHAchievementsData& mAchievementsData;
};
