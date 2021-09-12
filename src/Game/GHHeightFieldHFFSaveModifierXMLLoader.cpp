// Copyright Golden Hammer Software
#include "GHHeightFieldHFFSaveModifierXMLLoader.h"
#include "GHHeightFieldHFFSaveModifier.h"
#include "GHHeightFieldProperties.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"

GHHeightFieldHFFSaveModifierXMLLoader::GHHeightFieldHFFSaveModifierXMLLoader(const GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

void* GHHeightFieldHFFSaveModifierXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHHeightField* hf = (GHHeightField*)extraData.getProperty(GHHeightFieldProperties::HEIGHTFIELD);
	if (!hf) {
		GHDebugMessage::outputString("GHHeightFieldHFFSaveModifierXMLLoader called with no height field");
		return 0;
	}
	const char* filename = node.getAttribute("filename"); 
	if (!filename) filename = "heightfield.hff";
	GHHeightFieldHFFSaveModifier* ret = new GHHeightFieldHFFSaveModifier(*hf, mFileOpener, filename);
	return ret;
}
