// Copyright Golden Hammer Software
#include "GHPropertyBoolXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"
#include "GHPlatform/GHDebugMessage.h"

void* GHPropertyBoolXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* boolStr = node.getAttribute("val");
    if (!boolStr || !::strlen(boolStr)) {
        GHDebugMessage::outputString("No val specified for bool property");
        return 0;
    }
    bool boolVal = false;
    if (*boolStr == 't' || *boolStr == 'T') boolVal = true;
    return new GHProperty(boolVal);
}
