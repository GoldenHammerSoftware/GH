// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHBBEventHandler.h"
#include <scoreloop/scoreloopcore.h>

//Handles the Blackberry API system events for Scoreloop.
// This is where our asynchronous callbacks are processed.
class GHBBScoreloopEventHandler : public GHBBEventHandler
{
public:
	GHBBScoreloopEventHandler(SC_InitData_t& initData);
	virtual bool handleBBEvent(bps_event_t* event, int domain);

private:
	SC_InitData_t mInitData;
};
