// Copyright Golden Hammer Software
#include "GHMessage.h"

GHMessage::GHMessage(GHIdentifier type, GHPropertyContainer* data)
:	mMessageType(type)
{
    if (data) mPayload = *data;
}
