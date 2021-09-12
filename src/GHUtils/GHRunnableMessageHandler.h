// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

// A message handler that listens for a message that contains a runnable
//  and calls run on it.
class GHRunnableMessageHandler : public GHMessageHandler
{
public:
    virtual void handleMessage(const GHMessage& message);
};
