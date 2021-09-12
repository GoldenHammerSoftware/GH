// Copyright Golden Hammer Software
#include "GHRunnableMessageHandler.h"
#include "GHUtilsIdentifiers.h"
#include "GHMessage.h"
#include "GHPlatform/GHRunnable.h"

void GHRunnableMessageHandler::handleMessage(const GHMessage& message)
{
    GHRunnable* runnable = (GHRunnable*)message.getPayload().getProperty(GHUtilsIdentifiers::RUNNABLE);
    if (!runnable) {
        return;
    }
    runnable->run();
}

