// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopEventHandler.h"

GHBBScoreloopEventHandler::GHBBScoreloopEventHandler(SC_InitData_t& initData)
: mInitData(initData)
{

}

bool GHBBScoreloopEventHandler::handleBBEvent(bps_event_t* event, int domain)
{
	bool ret = SC_HandleBPSEvent(&mInitData, event) == BPS_SUCCESS;
	return ret;
}

