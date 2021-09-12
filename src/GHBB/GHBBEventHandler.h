// Copyright 2010 Golden Hammer Software
#pragma once

#include <vector>
#include <bps/event.h>

class GHBBEventHandler
{
public:
	virtual ~GHBBEventHandler(void) { }

	virtual bool handleBBEvent(bps_event_t* event, int domain) = 0;
};

typedef std::vector<GHBBEventHandler*> GHBBEventHandlerList;
