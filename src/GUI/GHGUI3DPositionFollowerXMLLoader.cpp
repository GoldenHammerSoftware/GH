// Copyright Golden Hammer Software
#include "GHGUI3DPositionFollowerXMLLoader.h"
#include "GHGUI3DPositionFollower.h"
#include "GHXMLNode.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUI3DPositionFollowerXMLLoader::GHGUI3DPositionFollowerXMLLoader(const GHPropertyContainer& props,
																   const GHViewInfo& viewInfo,
																   const GHStringIdFactory& stringHashtable)
: mProps(props)
, mViewInfo(viewInfo)
, mStringHashtable(stringHashtable)
{

}

void* GHGUI3DPositionFollowerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIPanel* parentWidget = (GHGUIPanel*)extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
	if (!parentWidget) {
		GHDebugMessage::outputString("Failed to load GUI3DPositionFollower: no parent widget");
		return 0;
	}

	GHIdentifier positionIndex;
	node.readAttrIdentifier("prop", positionIndex, mStringHashtable);

	return new GHGUI3DPositionFollower(mProps, positionIndex, mViewInfo, *parentWidget);
}
