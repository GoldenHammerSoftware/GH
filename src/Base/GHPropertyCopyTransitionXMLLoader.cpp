// Copyright Golden Hammer Software
#include "GHPropertyCopyTransitionXMLLoader.h"
#include "GHUtils/GHPropertyCopyTransition.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHPropertyCopyTransitionXMLLoader::GHPropertyCopyTransitionXMLLoader(GHPropertyContainer& destContainer, const GHStringIdFactory& hashTable)
: mDestContainer(destContainer)
, mIdFactory(hashTable)
{
}

void* GHPropertyCopyTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* sourceString = node.getAttribute("sourceId");
    const char* destString = node.getAttribute("destId");
    if (!sourceString || !destString) 
    {
        GHDebugMessage::outputString("Couldn't find source or dest id for prop copy");
        return 0;
    }
    GHIdentifier sourceId = mIdFactory.generateHash(sourceString);
    GHIdentifier destId = mIdFactory.generateHash(destString);
    
    GHPropertyContainer* sourceContainer = (GHPropertyContainer*)extraData.getProperty(GHUtilsIdentifiers::PROPS);
    if (!sourceContainer)
    {
        GHDebugMessage::outputString("couldn't find source container for prop copy");
        return 0;
    }
    
    return new GHPropertyCopyTransition(*sourceContainer, sourceId, mDestContainer, destId);
}
