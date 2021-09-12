// Copyright Golden Hammer Software
#include "GHMessageXMLLoader.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPropertyContainerXMLLoader.h"
#include "GHUtils/GHMessage.h"
#include "GHXMLNode.h"

GHMessageXMLLoader::GHMessageXMLLoader(const GHPropertyContainerXMLLoader& propLoader,
                                       const GHStringIdFactory& stringHash)
: mPropLoader(propLoader)
, mStringHash(stringHash)
{
}

void* GHMessageXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* idString = node.getAttribute("id");
    if (!idString) return 0;
    GHMessage* ret = new GHMessage(mStringHash.generateHash(idString));
    populate(ret, node, extraData);
    return ret;
}

void GHMessageXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMessage* ret = (GHMessage*)obj;
    mPropLoader.populate(&ret->getPayload(), node, extraData);
}
