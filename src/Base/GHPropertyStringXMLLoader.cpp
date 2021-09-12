// Copyright Golden Hammer Software
#include "GHPropertyStringXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"
#include "GHString/GHString.h"

void* GHPropertyStringXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* strval = node.getAttribute("val");
    if (!strval) return 0;
    GHString* retval = new GHString(strval, GHString::CHT_COPY);
    return new GHProperty(retval->getChars(), new GHRefCountedType<GHString>(retval));
}
