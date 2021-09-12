// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHGUIWidget;
class GHControllerMgr;
class GHController;

// A transition that adds a controller based on widget depth.
// Not to be confused with GHGUITransitionController which manages transitions between gui panels.
class GHGUIControllerTransition : public GHTransition
{
public:
	GHGUIControllerTransition(GHControllerMgr& controllerMgr, GHController* controller, const GHGUIWidget& parent);
	~GHGUIControllerTransition(void);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHControllerMgr& mControllerMgr;
	GHController* mController;
	const GHGUIWidget& mParent;
};

