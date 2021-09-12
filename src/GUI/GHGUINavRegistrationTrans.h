// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHGUINavMgr;
class GHGUIButton;

//handles registering/unregistering gui elements from the nav manager
class GHGUINavRegistrationTrans : public GHTransition
{
public:
	GHGUINavRegistrationTrans(GHGUINavMgr& navMgr, GHGUIButton& widget);
	~GHGUINavRegistrationTrans(void);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHGUINavMgr& mNavMgr;
	GHGUIButton& mWidget;
	bool mIsRegistered;
};
