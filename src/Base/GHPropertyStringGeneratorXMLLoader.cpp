// Copyright Golden Hammer Software
#include "GHPropertyStringGeneratorXMLLoader.h"
#include "GHUtils/GHPropertyStringGenerator.h"
#include "GHXMLNode.h"

GHPropertyStringGeneratorXMLLoader::GHPropertyStringGeneratorXMLLoader(const GHPropertyContainer& propertyContainer,
                                                                       const GHStringIdFactory& hashTable)
: mPropertyContainer(propertyContainer)
, mIdFactory(hashTable)
{
}

void* GHPropertyStringGeneratorXMLLoader::create(const GHXMLNode& node, 
                                                 GHPropertyContainer& extraData) const
{
	int bufSize = 128;
	node.readAttrInt("bufferSize", bufSize);
	
    GHPropertyStringGenerator* ret = new GHPropertyStringGenerator(mPropertyContainer, bufSize);
    
	const char* sourceString = node.getAttribute("text");
	ret->setSourceString(sourceString);
    
	const GHXMLNode* propsNode = node.getChild("monitorProperties", false);
	if (propsNode)
	{
		for (size_t i = 0; i < propsNode->getChildren().size(); ++i)
		{
			const char* name = propsNode->getChildren()[i]->getName();
            ret->addMonitoredProperty(mIdFactory.generateHash(name));
		}
	}
	return ret;
}
