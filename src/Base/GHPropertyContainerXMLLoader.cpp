// Copyright Golden Hammer Software
#include "GHPropertyContainerXMLLoader.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHProperty.h"
#include "GHString/GHStringIdFactory.h"

GHPropertyContainerXMLLoader::GHPropertyContainerXMLLoader(GHXMLObjFactory& objFactory,
                                                           const GHStringIdFactory& hashTable)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
{
}

void* GHPropertyContainerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPropertyContainer* ret = new GHPropertyContainer;
    populate(ret, node, extraData);
    return ret;
}

void GHPropertyContainerXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPropertyContainer& props = *((GHPropertyContainer*)obj);
    
    for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        const GHXMLNode* propNode = node.getChildren()[i];

        const char* propNameStr = propNode->getAttribute("name");
        if (!propNameStr)
        {
            GHDebugMessage::outputString("Prop with no name, skipping.");
            continue;
        }
        if (!propNode->getChildren().size()) 
        {
            GHDebugMessage::outputString("Prop with no val, skipping.");
            continue;
        }
        GHProperty* propVal = (GHProperty*)mObjFactory.load(*propNode->getChildren()[0], &extraData);
        if (!propVal)
        {
            GHDebugMessage::outputString("Unable to load prop val, skipping.");
            continue;
        }
        
        GHIdentifier propNameId = mIdFactory.generateHash(propNameStr);
        props.setProperty(propNameId, *propVal);
        delete propVal;
    }
}

