// Copyright Golden Hammer Software
#include "GHMaterialPassOverrideXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHString/GHStringIdFactory.h"
#include "GHMDesc.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMaterial.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHRenderProperties.h"

GHMaterialPassOverrideXMLLoader::GHMaterialPassOverrideXMLLoader(GHXMLObjFactory& objFactory,
                                                                 GHStringIdFactory& hashTable)
: mXMLObjFactory(objFactory)
, mIdFactory(hashTable)
{
}

void* GHMaterialPassOverrideXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc::PassOverrideList* ret = new GHMDesc::PassOverrideList;
    populate(ret, node, extraData);
    return ret;
}

void GHMaterialPassOverrideXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	// label the child loads as material pass override
	// this can tell the child loaders to not warn if there are fewer arguments supported on the new mat.
	GHPropertyStacker overrideStacker(extraData, GHRenderProperties::GP_LOADINGMATERIALOVERRIDE, 1);

    GHMDesc::PassOverrideList* ret = (GHMDesc::PassOverrideList*)obj;
    for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        if (!node.getChildren()[i]->getChildren().size()) {
            GHDebugMessage::outputString("Mat replace with no mat specified");
            continue;
        }
        const char* passStr = node.getChildren()[i]->getAttribute("pass");
        if (!passStr) {
            GHDebugMessage::outputString("No pass specified for mat override");
            continue;
        }
        GHIdentifier pass = mIdFactory.generateHash(passStr);
        GHMaterial* mat = (GHMaterial*)mXMLObjFactory.load(*node.getChildren()[i]->getChildren()[0], &extraData);
        if (!mat) {
            GHDebugMessage::outputString("Failed to load mat for pass");
            continue;
        }
        mat->acquire();
        (*ret)[pass] = mat;
    }
}
