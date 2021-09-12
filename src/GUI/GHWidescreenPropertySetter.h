// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHScreenInfo;
class GHEventMgr;
class GHPropertyContainer;

//Monitors the WINDOWRESIZE message and uses the aspect ratio of the
// screen info to set the GP_WIDESCREEN property
class GHWidescreenPropertySetter : public GHMessageHandler
{
public:
	GHWidescreenPropertySetter(GHEventMgr& eventMgr,
								const GHScreenInfo& screenInfo,
								GHPropertyContainer& properties);
	~GHWidescreenPropertySetter(void);

	virtual void handleMessage(const GHMessage& message);

private:
	void setWidescreenProperty(void);

private:
	GHEventMgr& mEventMgr;
	const GHScreenInfo& mScreenInfo;
	GHPropertyContainer& mProperties;
};
