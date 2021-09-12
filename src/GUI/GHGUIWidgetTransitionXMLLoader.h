// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUIMgr;
class GHResourceFactory;
class GHXMLObjFactory;
class GHGUITransitionDescXMLLoader;

/*
 // supports either loading a widget from file or inline from children.
 // if addToParent is specified true, add to whatever widget was being loaded
 //  addToParent only works when loading from a gui panel.
 <guiWidgetTransition widget="blah.xml" transitionProp=LP_TILE1 transitionTime=0 transitionWait=0/>
 <guiWidgetTransition addToParent=false>
    <guiPanel/>
 </guiWidgetTransition>
 */
class GHGUIWidgetTransitionXMLLoader : public GHXMLObjLoader
{
public:  
    GHGUIWidgetTransitionXMLLoader(GHGUIMgr& guiMgr, GHResourceFactory& resFactory, 
		GHXMLObjFactory& objFactory, const GHGUITransitionDescXMLLoader& tDescLoader);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHGUIMgr& mGUIMgr;
    GHResourceFactory& mResFactory;
    GHXMLObjFactory& mObjFactory;
    const GHGUITransitionDescXMLLoader& mTDescLoader;
};
