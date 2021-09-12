// Copyright Golden Hammer Software
#include "GHWindowListener.h"
#include "GHBaseIdentifiers.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHWindow.h"
#include "GHUtils/GHEventMgr.h"
#include "GHPlatform/GHDebugMessage.h"

GHWindowListener::GHWindowListener(GHWindow& window, GHPropertyContainer& props, GHEventMgr& eventMgr)
: mWindow(window)
, mProps(props)
, mEventMgr(eventMgr)
{
	mEventMgr.registerListener(GHBaseIdentifiers::M_TOGGLEFULLSCREEN, *this);
	mEventMgr.registerListener(GHBaseIdentifiers::M_FULLSCREENENABLED, *this);
	mEventMgr.registerListener(GHBaseIdentifiers::M_FULLSCREENDISABLED, *this);
}

GHWindowListener::~GHWindowListener(void)
{
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_TOGGLEFULLSCREEN, *this);
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_FULLSCREENENABLED, *this);
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_FULLSCREENDISABLED, *this);
}

void GHWindowListener::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHBaseIdentifiers::M_TOGGLEFULLSCREEN)
	{
		bool currFullscreen = (bool)mProps.getProperty(GHBaseIdentifiers::ISFULLSCREEN);
		mWindow.setFullscreen(!currFullscreen);
	}
	else if (message.getType() == GHBaseIdentifiers::M_FULLSCREENENABLED)
	{
		mProps.setProperty(GHBaseIdentifiers::ISFULLSCREEN, true);
	}
	else if (message.getType() == GHBaseIdentifiers::M_FULLSCREENDISABLED)
	{
		mProps.setProperty(GHBaseIdentifiers::ISFULLSCREEN, false);
	}
}
