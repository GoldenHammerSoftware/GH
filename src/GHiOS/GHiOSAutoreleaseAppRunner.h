#pragma once

#include "GHAppRunner.h"

class GHiOSAutoreleaseAppRunner : public GHAppRunner
{
public:
    GHiOSAutoreleaseAppRunner(GHApp* app,
                GHMessageHandler& appEventHandler, GHMessageQueue& messageQueue,
                GHInputState& gameInput, GHInputState& volatileInput,
                GHMutex& inputMutex, const GHThreadFactory& threadFactory,
                GHTransition* updatingTransition);
    
    virtual void runFrame(void) override;
};

