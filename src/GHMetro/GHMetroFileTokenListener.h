// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;

// listens for M_RELEASEFILETOKEN to free up future access list items.
class GHMetroFileTokenListener : public GHMessageHandler
{
public:
	GHMetroFileTokenListener(GHEventMgr& eventMgr);
	~GHMetroFileTokenListener(void);

	virtual void handleMessage(const GHMessage& message);

private:
	GHEventMgr& mEventMgr;
};