// Copyright Golden Hammer Software
#include "GHTransitionMessageHandler.h"
#include "GHUtils/GHTransition.h"

GHTransitionMessageHandler::GHTransitionMessageHandler(GHTransition* transition)
: mTransition(transition)
{
    
}

GHTransitionMessageHandler::~GHTransitionMessageHandler(void)
{
    
}

void GHTransitionMessageHandler::handleMessage(const GHMessage& message)
{
    mTransition->activate();
}
