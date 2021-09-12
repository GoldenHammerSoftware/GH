// Copyright Golden Hammer Software
#include "GHPropertyIdentifierXMLLoader.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProperty.h"

GHPropertyIdentifierXMLLoader::GHPropertyIdentifierXMLLoader(GHStringIdFactory& hashTable)
: mIdFactory(hashTable)
{
}

void* GHPropertyIdentifierXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* valString = node.getAttribute("val");
    if (!valString) {
        GHDebugMessage::outputString("No value for identifier prop");
        return 0;
    }
    GHIdentifier valId = mIdFactory.generateHash(valString);
    GHIdentifier* propId = new GHIdentifier(valId);
    GHProperty* ret = new GHProperty(propId, new GHRefCountedType<GHIdentifier>(propId));
    return ret;
}

