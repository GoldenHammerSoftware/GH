// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHTransition;

// A message handler that triggers a transition upon receiving the message
class GHTransitionMessageHandler : public GHMessageHandler
{
public:
    GHTransitionMessageHandler(GHTransition* transition);
    ~GHTransitionMessageHandler(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHTransition* mTransition;
};
