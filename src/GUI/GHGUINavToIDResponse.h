// Copyright Golden Hammer Software
#pragma once

#include "GHGUINavResponse.h"
#include "GHString/GHIdentifier.h"

class GHGUINavMgr;

class GHGUINavToIDResponse : public GHGUINavResponse
{
public:
	GHGUINavToIDResponse(GHGUINavMgr& navMgr, const GHIdentifier& identifier);

	virtual bool respondToNavigation(const GHPoint2i& navDir);

private:
	GHGUINavMgr& mNavMgr;
	GHIdentifier mId;
};
