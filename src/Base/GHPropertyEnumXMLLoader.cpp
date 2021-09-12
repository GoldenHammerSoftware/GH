// Copyright Golden Hammer Software
#include "GHPropertyEnumXMLLoader.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProperty.h"

GHPropertyEnumXMLLoader::GHPropertyEnumXMLLoader(const GHIdentifierMap<int>& enumStore)
: mEnumStore(enumStore)
{
}

void* GHPropertyEnumXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* valString = node.getAttribute("val");
    if (!valString) {
        GHDebugMessage::outputString("No value for enum prop");
        return 0;
    }
    const int* val = mEnumStore.find(valString);
    if (!val) {
        GHDebugMessage::outputString("No enum found for val %s", valString);
        return 0;
    }
    return new GHProperty(*val);
}
