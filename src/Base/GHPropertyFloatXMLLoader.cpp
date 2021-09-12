// Copyright Golden Hammer Software
#include "GHPropertyFloatXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"

void* GHPropertyFloatXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    float val = 0;
    node.readAttrFloat("val", val);
    return new GHProperty(val);
}
