// Copyright Golden Hammer Software
#include "GHMaterialReplacerXMLLoader.h"
#include "Base/GHXMLNode.h"
#include "GHMaterialReplacer.h"
#include "GHString/GHStringIdFactory.h"
#include "Base/GHXMLObjFactory.h"
#include "GHString/GHHashListParser.h"
#include <string.h>

GHMaterialReplacerXMLLoader::GHMaterialReplacerXMLLoader(GHXMLObjFactory& objFactory,
                                                         GHStringIdFactory& hashTable)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
{
}

void* GHMaterialReplacerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMaterialReplacer* ret = new GHMaterialReplacer;
    populate(ret, node, extraData);
    return ret;
}

void GHMaterialReplacerXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMaterialReplacer* ret = (GHMaterialReplacer*)obj;
    
    const GHXMLNode::NodeList& replacements = node.getChildren();
    for (size_t i = 0; i < replacements.size(); ++i)
    {
        if (!replacements[i]->getChildren().size()) continue;
        const char* replacestr = replacements[i]->getAttribute("replace");
        GHMaterial* mat = (GHMaterial*)mObjFactory.load(*replacements[i]->getChildren()[0]);
        if (mat)
        {
            if (!::strcmp(replacestr, "default"))
            {
                ret->setDefaultReplacement(mat);
            }
            else
            {
                std::vector<GHIdentifier> ids;
                GHHashListParser::parseHashList(replacestr, mIdFactory, ids);
                for (size_t idIdx = 0; idIdx < ids.size(); ++idIdx)
                {
                    ret->addReplacement(mat, ids[idIdx]);
                }
            }
        }
    }
}
