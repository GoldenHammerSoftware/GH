// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHTimeVal;
class GHControllerMgr;

// <guiSizeOscillate speed=1.0 max=0.5/>
class GHGUISizeOscillateTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHGUISizeOscillateTransitionXMLLoader(const GHTimeVal& time, GHControllerMgr& controllerMgr);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHTimeVal& mTime;
	GHControllerMgr& mControllerMgr;
};
