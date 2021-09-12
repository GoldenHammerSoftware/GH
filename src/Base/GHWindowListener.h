// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHWindow;
class GHPropertyContainer;
class GHEventMgr;

// listens for messages related to window management
class GHWindowListener : public GHMessageHandler
{
public:
	GHWindowListener(GHWindow& window, GHPropertyContainer& props, GHEventMgr& eventMgr);
	~GHWindowListener(void);

	virtual void handleMessage(const GHMessage& message);

private:
	GHWindow& mWindow;
	GHPropertyContainer& mProps;
	GHEventMgr& mEventMgr;
};
