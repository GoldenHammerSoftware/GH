// Copyright 2010 Golden Hammer Software
#include "GHCocoaURLOpenerTransitionXMLLoader.h"
#include "GHCocoaURLOpenerTransition.h"
#include "GHDebugMessage.h"
#include "GHXMLNode.h"

void* GHCocoaURLOpenerTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* url = node.getAttribute("url");
    if (!url) {
        GHDebugMessage::outputString("URLTrans with no url specified");
        return 0;
    }
    return new GHCocoaURLOpenerTransition(url);
}
