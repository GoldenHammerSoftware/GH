// Copyright Golden Hammer Software
#include "GHGUINavRegistrationTrans.h"
#include "GHGUINavMgr.h"
#include "GHGUIButton.h"

GHGUINavRegistrationTrans::GHGUINavRegistrationTrans(GHGUINavMgr& navMgr, GHGUIButton& widget)
: mNavMgr(navMgr)
, mWidget(widget)
, mIsRegistered(false)
{

}

GHGUINavRegistrationTrans::~GHGUINavRegistrationTrans(void)
{
	
}

void GHGUINavRegistrationTrans::activate(void)
{
	if (mWidget.getNavigator().isNavigable()) {
		mNavMgr.registerElement(mWidget);
		mIsRegistered = true;
	}
}

void GHGUINavRegistrationTrans::deactivate(void)
{
	if (mIsRegistered) {
		mNavMgr.unregisterElement(mWidget);
		mIsRegistered = false;
	}
}
