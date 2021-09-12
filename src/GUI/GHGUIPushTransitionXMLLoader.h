// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUIMgr;
class GHXMLObjFactory;
class GHResourceFactory;
class GHStringIdFactory;
class GHGUITransitionDescXMLLoader;

/*
 // supports either loading a widget from file or inline from children.
 <guiPush widget="blah.xml" loadOnDemand=true transitionProp=LP_TILE1 transitionTime=0 transitionWait=0/>
 <guiPush>
    <guiPanel/>
 </guiPush>
*/
class GHGUIPushTransitionXMLLoader : public GHXMLObjLoader
{
public: 
    GHGUIPushTransitionXMLLoader(GHGUIMgr& guiMgr, GHResourceFactory& resourceFactory,
                                 GHXMLObjFactory& objFactory,
								 const GHGUITransitionDescXMLLoader& tDescLoader);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHGUIMgr& mGUIMgr;
    GHResourceFactory& mResourceFactory;
    GHXMLObjFactory& mObjFactory;
	const GHGUITransitionDescXMLLoader& mTDescLoader;
};
