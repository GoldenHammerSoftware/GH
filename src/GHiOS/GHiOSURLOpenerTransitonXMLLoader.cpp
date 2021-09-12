// Copyright 2010 Golden Hammer Software
#include "GHiOSURLOpenerTransitonXMLLoader.h"
#include "GHiOSURLOpenerTransiton.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"

void* GHiOSURLOpenerTransitonXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* url = node.getAttribute("url");
    if (!url) {
        GHDebugMessage::outputString("URLTrans with no url specified");
        return 0;
    }
    return new GHiOSURLOpenerTransiton(url);
}
