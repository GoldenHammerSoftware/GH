// Copyright Golden Hammer Software
#include "GHGUINavToIDResponse.h"
#include "GHGUINavMgr.h"

GHGUINavToIDResponse::GHGUINavToIDResponse(GHGUINavMgr& navMgr, const GHIdentifier& identifier)
	: mNavMgr(navMgr)
	, mId(identifier)
{

}

bool GHGUINavToIDResponse::respondToNavigation(const GHPoint2i& navDir)
{
	return mNavMgr.select(mId);
}

