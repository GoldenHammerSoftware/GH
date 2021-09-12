#include "GHiOSAutoreleaseAppRunner.h"

GHiOSAutoreleaseAppRunner::GHiOSAutoreleaseAppRunner(GHApp* app,
            GHMessageHandler& appEventHandler, GHMessageQueue& messageQueue,
            GHInputState& gameInput, GHInputState& volatileInput,
            GHMutex& inputMutex, const GHThreadFactory& threadFactory,
            GHTransition* updatingTransition)
: GHAppRunner(app, appEventHandler, messageQueue, gameInput, volatileInput, inputMutex, threadFactory, updatingTransition)
{
}

void GHiOSAutoreleaseAppRunner::runFrame(void)
{
    @autoreleasepool {
        GHAppRunner::runFrame();
    }
}
