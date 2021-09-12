// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;
class GHEventMgr;

// A handler to listen for a message to set a property.
// We expect the message to have two arguments:
//  GHUtilsIdentifiers::PROPID - id of the prop to set
//  GHUtilsIdentifiers::PROPVAL - val to copy.
class GHPropertySetMessageHandler : public GHMessageHandler
{
public:
    GHPropertySetMessageHandler(GHPropertyContainer& props, const GHIdentifier& messageId, GHEventMgr& eventMgr);
    ~GHPropertySetMessageHandler(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHPropertyContainer& mProps;
    GHEventMgr& mEventMgr;
    GHIdentifier mMessageId;
};
