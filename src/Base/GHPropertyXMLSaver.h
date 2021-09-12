// Copyright Golden Hammer Software
#pragma once

#include <stdio.h>
#include "GHXMLObjLoader.h"

// dumping ground for writing properties to xml nodes.
class GHPropertyXMLSaver
{
public:

	//todo: unify saveFloatProp and saveBoolProp with saveProp
    static void saveFloatProp(GHXMLNode& topNode, const GHPropertyContainer& props,
                              const GHIdentifier& id, const char* propName)
    {
        GHXMLNode* propTop = new GHXMLNode();
        topNode.addChild(propTop);
        propTop->setName("prop", GHString::CHT_REFERENCE);
        propTop->setAttribute("name", GHString::CHT_REFERENCE, propName, GHString::CHT_REFERENCE);
        
        GHXMLNode* valNode = new GHXMLNode();
        propTop->addChild(valNode);
        valNode->setName("floatProp", GHString::CHT_COPY);
        char buffer[64];
        sprintf(buffer, "%f", (float)props.getProperty(id));
        valNode->setAttribute("val", GHString::CHT_REFERENCE, buffer, GHString::CHT_COPY);
    }
    
    static void saveBoolProp(GHXMLNode& topNode, const GHPropertyContainer& props,
                             const GHIdentifier& id, const char* propName)
    {
        GHXMLNode* propTop = new GHXMLNode();
        topNode.addChild(propTop);
        propTop->setName("prop", GHString::CHT_REFERENCE);
        propTop->setAttribute("name", GHString::CHT_REFERENCE, propName, GHString::CHT_REFERENCE);
        
        GHXMLNode* valNode = new GHXMLNode();
        propTop->addChild(valNode);
        valNode->setName("boolProp", GHString::CHT_COPY);
        bool boolval = props.getProperty(id);
        const char* valStr = "true";
        if (!boolval) valStr = "false";
        valNode->setAttribute("val", GHString::CHT_REFERENCE, valStr, GHString::CHT_REFERENCE);
    }

	static void saveProp(GHXMLNode& topNode, const GHPropertyContainer& props, const GHIdentifier& id, 
						 const char* propName, const char* propTypeName, const GHXMLObjLoader& saver)
	{
		GHXMLNode* propTop = new GHXMLNode();
		topNode.addChild(propTop);
		propTop->setName("prop", GHString::CHT_REFERENCE);
		propTop->setAttribute("name", GHString::CHT_REFERENCE, propName, GHString::CHT_REFERENCE);

		GHXMLNode* valNode = new GHXMLNode();
		propTop->addChild(valNode);
		valNode->setName(propTypeName, GHString::CHT_REFERENCE);
		saver.save(*valNode, props.getProperty(id).getVal<void*>(), 0);
	}
};
