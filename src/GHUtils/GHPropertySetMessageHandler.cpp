// Copyright Golden Hammer Software
#include "GHPropertySetMessageHandler.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHPlatform/GHDebugMessage.h"

GHPropertySetMessageHandler::GHPropertySetMessageHandler(GHPropertyContainer& props, const GHIdentifier& messageId,
                                                         GHEventMgr& eventMgr)
: mProps(props)
, mMessageId(messageId)
, mEventMgr(eventMgr)
{
    mEventMgr.registerListener(mMessageId, *this);
}

GHPropertySetMessageHandler::~GHPropertySetMessageHandler(void)
{
    mEventMgr.unregisterListener(mMessageId, *this);
}

void GHPropertySetMessageHandler::handleMessage(const GHMessage& message)
{
    const GHProperty& propIdProp = message.getPayload().getProperty(GHUtilsIdentifiers::PROPID);
    GHIdentifier propId = 0;
    if (propIdProp.isValid()) {
        propId = *((GHIdentifier*)propIdProp);
    }
    else {
        GHDebugMessage::outputString("GHPropertySetMessageHandler: Message with no propId");
    }
    const GHProperty& propVal = message.getPayload().getProperty(GHUtilsIdentifiers::PROPVAL);
    mProps.setProperty(propId, propVal);
}
