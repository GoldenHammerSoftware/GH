// Copyright Golden Hammer Software
#include "GHPropertyIntXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"

void* GHPropertyIntXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    int val = 0;
    node.readAttrInt("val", val);
    return new GHProperty(val);
}
