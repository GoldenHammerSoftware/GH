// Copyright Golden Hammer Software
#include "GHPropertyCopyXMLLoader.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProperty.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyCopyXMLLoader::GHPropertyCopyXMLLoader(GHStringIdFactory& hashTable)
	: mIdFactory(hashTable)
{
}

// <copyProp src=id/>
void* GHPropertyCopyXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const char* srcString = node.getAttribute("src");
	if (!srcString) 
	{
		GHDebugMessage::outputString("No value string for copy prop");
		return 0;
	}
	GHIdentifier srcId = mIdFactory.generateHash(srcString);
	const GHProperty& srcProp = extraData.getProperty(srcId);
	if (!srcProp.isValid())
	{
		GHDebugMessage::outputString("No src prop found for copyProp");
		return 0;
	}

	GHProperty* ret = new GHProperty(srcProp);
	return ret;
}

