// Copyright Golden Hammer Software
#include "GHFontLoader.h"
#include "GHFont.h"
#include "Base/GHXMLNode.h"
#include "Base/GHXMLObjFactory.h"
#include "GHPlatform/GHDebugMessage.h"

GHFontLoader::GHFontLoader(const GHXMLObjFactory& objFactory)
: mObjFactory(objFactory)
{
}

void* GHFontLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const GHXMLNode* matParent = node.getChild("material", false);
    if (!matParent || !matParent->getChildren().size()) {
        GHDebugMessage::outputString("Failed to find font material node.");
        return 0;
    }
    GHMaterial* mat = (GHMaterial*)mObjFactory.load(*matParent->getChildren()[0]);
    if (!mat) {
        GHDebugMessage::outputString("Failed to loat font material.");
    }
    GHFont* ret = new GHFont(mat);
    populate(ret, node, extraData);
    return ret;
}

void GHFontLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHFont* ret = (GHFont*)obj;
    const GHXMLNode* letterParent = node.getChild("letters", false);
	if (!letterParent) {
		GHDebugMessage::outputString("Failed to find letters node for font");
		return;
	}
    GHRect<float, 2> letterPos;
    for (size_t i = 0; i < letterParent->getChildren().size(); ++i)
    {
        const GHXMLNode* currLetterNode = letterParent->getChildren()[i];
        const char* letterStr = currLetterNode->getAttribute("char");
        if (!letterStr) {
            continue;
        }
        currLetterNode->readAttrFloatArr("min", letterPos.mMin.getArr(), 2);
        currLetterNode->readAttrFloatArr("max", letterPos.mMax.getArr(), 2);
        ret->setLetterPosition(*letterStr, letterPos);
    }
}
