// Copyright Golden Hammer Software
#include "GHXMLRedirectLoader.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"

GHXMLRedirectLoader::GHXMLRedirectLoader(const GHXMLObjFactory& objFactory)
: mObjFactory(objFactory)
{
}

void* GHXMLRedirectLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* fileStr = node.getAttribute("file");
    if (!fileStr) {
        GHDebugMessage::outputString("No file specified for redirect xml");
        return 0;
    }
    return mObjFactory.load(fileStr, &extraData);
}
