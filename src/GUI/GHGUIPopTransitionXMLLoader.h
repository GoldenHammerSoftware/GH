// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUIMgr;
class GHGUITransitionDescXMLLoader;
class GHStringIdFactory;

/*
 <guiPop widget="blah.xml" transitionProp=LP_TILE1 transitionTime=0 transitionWait=0 addToStack=true category=0/>
 */
class GHGUIPopTransitionXMLLoader : public GHXMLObjLoader
{
public:  
    GHGUIPopTransitionXMLLoader(GHGUIMgr& guiMgr, const GHStringIdFactory& hashTable,
		const GHGUITransitionDescXMLLoader& tDescLoader);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHGUIMgr& mGUIMgr;
	const GHStringIdFactory& mIdFactory;
    const GHGUITransitionDescXMLLoader& mTDescLoader;
};
