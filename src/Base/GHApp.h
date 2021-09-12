// Copyright Golden Hammer Software
#pragma once

class GHMessageHandler;

// interface class for something the platform code can interact with.
class GHApp
{
public:
	virtual ~GHApp(void) {}
	
	// we call runFrame from outside every tick.
	// this is to support systems that use an event-based update such as mac carbon.
	virtual void runFrame(void) = 0;
};
