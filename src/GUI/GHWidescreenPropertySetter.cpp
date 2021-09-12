// Copyright Golden Hammer Software
#include "GHWidescreenPropertySetter.h"
#include "GHUtils/GHEventMgr.h"
#include "GHScreenInfo.h"
#include "GHMessageTypes.h"
#include "GHGUIProperties.h"
#include "GHUtils/GHPropertyContainer.h"

GHWidescreenPropertySetter::GHWidescreenPropertySetter(GHEventMgr& eventMgr,
														const GHScreenInfo& screenInfo,
														GHPropertyContainer& properties)
 : mEventMgr(eventMgr)
 , mScreenInfo(screenInfo)
 , mProperties(properties)
{
	setWidescreenProperty();
	mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, *this);
}

GHWidescreenPropertySetter::~GHWidescreenPropertySetter(void)
{
	mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, *this);
}

void GHWidescreenPropertySetter::handleMessage(const GHMessage& message)
{
	setWidescreenProperty();
}

void GHWidescreenPropertySetter::setWidescreenProperty(void)
{
	if (mScreenInfo.getAspectRatio() > 0.995f) {
		mProperties.setProperty(GHGUIProperties::GP_WIDESCREEN, true);
	}
	else {
		mProperties.setProperty(GHGUIProperties::GP_WIDESCREEN, false);
	}
}
