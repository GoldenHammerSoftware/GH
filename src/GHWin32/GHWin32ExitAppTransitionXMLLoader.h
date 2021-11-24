// Copyright Golden Hammer Software
#pragma once

#include "Base/GHXMLObjLoader.h"

// <exitApp/>
class GHWin32ExitAppTransitionXMLLoader : public GHXMLObjLoader
{
public:
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;
};
