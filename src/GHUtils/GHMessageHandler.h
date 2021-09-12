// Copyright Golden Hammer Software
#pragma once

class GHMessage;

// Interface for something to which messages can be sent.
class GHMessageHandler
{
public:
	virtual ~GHMessageHandler(void) {}
	
	virtual void handleMessage(const GHMessage& message) = 0;
};
