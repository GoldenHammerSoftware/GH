// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHPropertyContainer.h"

// A class for encapsulating an arbitrary message.
class GHMessage
{
public:
	GHMessage(GHIdentifier type, GHPropertyContainer* data=0);
	
	inline GHIdentifier getType(void) const { return mMessageType; }
	const GHPropertyContainer& getPayload(void) const { return mPayload; }
	GHPropertyContainer& getPayload(void) { return mPayload; }
    
protected:
	// type of message.  helps the app know what to expect from mData.
	// apps should use an enum value.
	GHIdentifier mMessageType;
	// Any additional arguments.  
    GHPropertyContainer mPayload;
};
