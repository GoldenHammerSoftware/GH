// Copyright Golden Hammer Software
#include "GHThreadUtil.h"

#include "GHUtilsMessageTypes.h"
#include "GHUtilsIdentifiers.h"
#include "GHMessage.h"
#include "GHMessageSenderTransition.h"
#include "GHPlatform/GHTypedVoidFuncRunnable.h"

GHTransition* GHThreadUtil::createMessageWrapperTransition(GHTransition& targetTrans, GHMessageHandler& handler)
{
    GHTypedVoidFuncRunnable<GHTransition>* activateRunnable = new GHTypedVoidFuncRunnable<GHTransition>(targetTrans, &GHTransition::activate);
    GHMessage* activateMessage = new GHMessage(GHUtilsMessageTypes::EXECRUNNABLE);
    activateMessage->getPayload().setProperty(GHUtilsIdentifiers::RUNNABLE,
                                              GHProperty(activateRunnable, new GHRefCountedType<GHTypedVoidFuncRunnable<GHTransition> >(activateRunnable)));
    
    GHTypedVoidFuncRunnable<GHTransition>* deactivateRunnable = new GHTypedVoidFuncRunnable<GHTransition>(targetTrans, &GHTransition::deactivate);
    GHMessage* deactivateMessage = new GHMessage(GHUtilsMessageTypes::EXECRUNNABLE);
    deactivateMessage->getPayload().setProperty(GHUtilsIdentifiers::RUNNABLE,
                                                GHProperty(deactivateRunnable, new GHRefCountedType<GHTypedVoidFuncRunnable<GHTransition> >(deactivateRunnable)));
    
    GHMessageSenderTransition* ret = new GHMessageSenderTransition(handler);
    ret->addActivateMessage(activateMessage);
    ret->addDeactivateMessage(deactivateMessage);
    return ret;
}
