// Copyright Golden Hammer Software
#include "GHGUIControllerTransition.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHController.h"
#include "GHGUIWidget.h"
#include "GHGUIController.h"

GHGUIControllerTransition::GHGUIControllerTransition(GHControllerMgr& controllerMgr, GHController* controller, const GHGUIWidget& parent)
	: mControllerMgr(controllerMgr)
	, mController(controller)
	, mParent(parent)
{
}

GHGUIControllerTransition::~GHGUIControllerTransition(void)
{
	delete mController;
}

void GHGUIControllerTransition::activate(void)
{
	mControllerMgr.add(mController, GHGUIController::calcPriority(mParent.getDepth()));
}

void GHGUIControllerTransition::deactivate(void)
{
	mControllerMgr.remove(mController);
}
