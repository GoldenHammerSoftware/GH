// Copyright Golden Hammer Software
#include "GHXMLObjLoader.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHDebugMessage.h"

void* GHXMLObjLoader::load(const GHXMLNode& node, GHPropertyContainer& extraData, const GHXMLObjFactory& objFactory) const
{
    const char* inherit = node.getAttribute("inherit");
    if (inherit) {
        void* ret = objFactory.load(inherit, &extraData);
        if (!ret) {
            GHDebugMessage::outputString("failed to load inherit object.");
            return 0;
        }
        populate(ret, node, extraData);
        return ret;
    }
    return create(node, extraData);
}

bool GHXMLObjLoader::save(GHXMLNode& node, void* data, GHPropertyContainer* extraData) const
{
	assert(false && "Error: GHXMLObjLoader::save called. You probably want to override it in the derived serializer you're expecting to save with");
	return false;
}
