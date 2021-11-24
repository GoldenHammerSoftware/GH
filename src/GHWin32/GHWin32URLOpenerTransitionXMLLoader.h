// Copyright Golden Hammer Software
#pragma once

#include "Base/GHXMLObjLoader.h"

// <openURL url="http://www.goldenhammersoftware.com"/>
class GHWin32URLOpenerTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHWin32URLOpenerTransitionXMLLoader(void);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;
};
